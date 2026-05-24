#include "Windows.h"
#include "Memory.h"
#include "Offsets.h"
#include "Overlay.h"
#include "Math.h"
// #include "iostream"
#include "vector"

MSG msg;
Overlay* overlay = NULL;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_DESTROY) { PostQuitMessage(0); return 0; }
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0, 0, hInstance, NULL, NULL, NULL, NULL, L"OverlayWindow", NULL };
	RegisterClassEx(&wc);

	HWND hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT, wc.lpszClassName, L"x2d hl2dm", WS_POPUP, 0, 0, screenWidth, screenHeight, NULL, NULL, hInstance, NULL);

	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
	ShowWindow(hwnd, SW_SHOW);

	overlay = new Overlay(hwnd);
	bool game = false;

	// Game process
	while (!game)
	{
		if (g_Memory.AttachProcess(L"hl2mp_win64.exe"))
		{
			game = true;
		}
		else
		{
			Sleep(1000);
		}
	}

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		// Game modules
		g_Memory.base_client_ = g_Memory.GetModule(L"client.dll");
		g_Memory.base_engine_ = g_Memory.GetModule(L"engine.dll");

		if (g_Memory.base_client_.found && g_Memory.base_engine_.found)
			break;
	}

	while (true)
	{
		overlay->BeginDraw();

		// Local Player
		uint64_t localPlayer = g_Memory.read<uint64_t>(g_Memory.base_client_.base + Offsets::localplayer);
		if (!localPlayer) { overlay->EndDraw(); continue; }

		// ViewMatrix
		float viewMatrix[16];
		for (int i = 0; i < 16; i++)
		{
			viewMatrix[i] = g_Memory.read<float>(g_Memory.base_engine_.base + Offsets::viewmatrix + (i * 4));
		}
		
		for (int i = 0; i < 64; i++)
		{
			uint64_t entity = g_Memory.read<uint64_t>(g_Memory.base_client_.base + Offsets::entitylist + (i * Offsets::playerpawn));

			if (!entity || entity == localPlayer) continue;

			int health = g_Memory.read<int>(entity + Offsets::health);

			if (health <= 0 || health > 500)
			{
				continue;
			}

			// Position
			Vector3 entPos;
			entPos.x = g_Memory.read<float>(entity + Offsets::position);
			entPos.y = g_Memory.read<float>(entity + Offsets::position + 4);
			entPos.z = g_Memory.read<float>(entity + Offsets::position + 8);

			Vector3 headPos = entPos;
			headPos.z += 72.0f;

			Vector3 screenPos, screenHead;
			if (WorldToScreen(entPos, screenPos, viewMatrix, screenWidth, screenHeight) && WorldToScreen(headPos, screenHead, viewMatrix, screenWidth, screenHeight))
			{
				float height = abs(screenPos.y - screenHead.y);
				float width = height / 2.0f;
				
				// Box
				overlay->DrawBox((int)(screenPos.x - width / 2), (int)(screenPos.y - height), (int)width, (int)height, RGB(221, 230, 75), 1, false);

				// Snapline
				overlay->DrawLine(screenWidth / 2, screenHeight, (int)screenPos.x, (int)screenPos.y, RGB(221, 230, 75), 1);

				// Health
				std::wstring healthStr = L"HP: " + std::to_wstring(health);
				overlay->DrawText(healthStr, (int)(screenPos.x - width / 2), (int)(screenPos.y - height - 20), RGB(221, 230, 75));
			}
		}

		overlay->EndDraw();
		Sleep(10);
	}

	delete overlay;
	return 0;
}