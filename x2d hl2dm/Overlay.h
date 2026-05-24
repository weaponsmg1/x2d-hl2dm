#pragma once
#include "Windows.h"
#include "string"

class Overlay
{
private:
	HWND hwnd;
	HDC hdcMem = nullptr;
	HBITMAP hBitmap = nullptr;
	int lastWidth = 0;
	int lastHeight = 0;

public:
	Overlay(HWND hwnd);
	~Overlay();

	void BeginDraw();
	void EndDraw();
	void DrawText(const std::wstring& text, int x, int y, COLORREF color, HFONT hFont = nullptr);
	void DrawLine(int x1, int y1, int x2, int y2, COLORREF color, int strokeWidth = 1);
	void DrawBox(int x, int y, int width, int height, COLORREF color, int strokeWidth = 1, bool outlineEnabled = false);
	void DrawFilledBox(int x, int y, int width, int height, COLORREF fillColor, bool outlineEnabled = false, COLORREF outlineColor = RGB(0, 0, 0));
	void DrawCircle(int x, int y, int radius, COLORREF color, int strokeWidth);
	HFONT CreateFontScaled(float height);
	SIZE GetTextSize(const std::wstring& text, HFONT font);
};

