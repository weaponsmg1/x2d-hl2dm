#include "Overlay.h"

Overlay::Overlay(HWND hwnd) : hwnd(hwnd)
{
	HDC hdc = GetDC(hwnd);
	hdcMem = CreateCompatibleDC(hdc);
	ReleaseDC(hwnd, hdc);
}

Overlay::~Overlay()
{
	if (hBitmap) DeleteObject(hBitmap);
	if (hdcMem) DeleteDC(hdcMem);
}

void Overlay::BeginDraw()
{
	RECT rect;
	GetClientRect(hwnd, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	if (width != lastWidth || height != lastHeight) {
		if (hBitmap) DeleteObject(hBitmap);
		HDC hdc = GetDC(hwnd);
		hBitmap = CreateCompatibleBitmap(hdc, width, height);
		ReleaseDC(hwnd, hdc);
		SelectObject(hdcMem, hBitmap);
		lastWidth = width;
		lastHeight = height;
	}

	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
	FillRect(hdcMem, &rect, hBrush);
	DeleteObject(hBrush);
}

void Overlay::EndDraw()
{
	RECT rect;
	GetClientRect(hwnd, &rect);
	HDC hdc = GetDC(hwnd);
	BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);
	ReleaseDC(hwnd, hdc);
}

void Overlay::DrawText(const std::wstring& text, int x, int y, COLORREF color, HFONT hFont)
{
	SetTextColor(hdcMem, color);
	SetBkMode(hdcMem, TRANSPARENT);
	HFONT oldFont = nullptr;
	if (hFont) oldFont = (HFONT)SelectObject(hdcMem, hFont);
	TextOutW(hdcMem, x, y, text.c_str(), (int)text.length());
	if (oldFont) SelectObject(hdcMem, oldFont);
}

HFONT Overlay::CreateFontScaled(float height)
{
	int fontSize = (int)(height / 8.0f);
	if (fontSize < 10) fontSize = 10;
	if (fontSize > 24) fontSize = 24;

	return CreateFontW(fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, FIXED_PITCH | FF_DONTCARE, L"Courier New");

}

SIZE Overlay::GetTextSize(const std::wstring& text, HFONT font)
{
	SIZE size = {};
	HFONT oldFont = nullptr;

	if (font) oldFont = (HFONT)SelectObject(hdcMem, font);
	GetTextExtentPoint32W(hdcMem, text.c_str(), (int)text.length(), &size);
	if (oldFont) SelectObject(hdcMem, oldFont);

	return size;
}



void Overlay::DrawLine(int x1, int y1, int x2, int y2, COLORREF color, int strokeWidth)
{
	HPEN hPen = CreatePen(PS_SOLID, strokeWidth, color);
	HPEN oldPen = (HPEN)SelectObject(hdcMem, hPen);
	MoveToEx(hdcMem, x1, y1, NULL);
	LineTo(hdcMem, x2, y2);
	SelectObject(hdcMem, oldPen);
	DeleteObject(hPen);
}

void Overlay::DrawBox(int x, int y, int width, int height, COLORREF color, int strokeWidth, bool outlineEnabled)
{
	HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdcMem, hBrush);

	HPEN oldPen = nullptr;

	if (outlineEnabled) {
		COLORREF outlineColor = RGB(1, 1, 1);
		int outlineOffset = 1;

		HPEN hOutlinePen = CreatePen(PS_SOLID, strokeWidth, outlineColor);
		oldPen = (HPEN)SelectObject(hdcMem, hOutlinePen);

		Rectangle(hdcMem, x - outlineOffset, y - outlineOffset, x + width + outlineOffset, y + height + outlineOffset);

		DeleteObject(SelectObject(hdcMem, oldPen));
	}

	HPEN hPen = CreatePen(PS_SOLID, strokeWidth, color);
	oldPen = (HPEN)SelectObject(hdcMem, hPen);

	Rectangle(hdcMem, x, y, x + width, y + height);

	SelectObject(hdcMem, oldBrush);
	SelectObject(hdcMem, oldPen);

	DeleteObject(hPen);
}


void Overlay::DrawFilledBox(int x, int y, int width, int height, COLORREF fillColor, bool outlineEnabled, COLORREF outlineColor)
{
	if (outlineEnabled)
	{
		HPEN hOutlinePen = CreatePen(PS_SOLID, 1, outlineColor);
		HPEN oldPen = (HPEN)SelectObject(hdcMem, hOutlinePen);
		HBRUSH hNullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
		HBRUSH oldBrush = (HBRUSH)SelectObject(hdcMem, hNullBrush);

		Rectangle(hdcMem, x - 1, y - 1, x + width + 1, y + height + 1);

		SelectObject(hdcMem, oldBrush);
		SelectObject(hdcMem, oldPen);
		DeleteObject(hOutlinePen);
	}

	HBRUSH hBrush = CreateSolidBrush(fillColor);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdcMem, hBrush);
	Rectangle(hdcMem, x, y, x + width, y + height);
	SelectObject(hdcMem, oldBrush);
	DeleteObject(hBrush);
}

void Overlay::DrawCircle(int x, int y, int radius, COLORREF color, int strokeWidth)
{
	HPEN hPen = CreatePen(PS_SOLID, strokeWidth, color);
	HPEN oldPen = (HPEN)SelectObject(hdcMem, hPen);
	HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdcMem, hBrush);

	Ellipse(hdcMem, x - radius, y - radius, x + radius, y + radius);

	SelectObject(hdcMem, oldBrush);
	SelectObject(hdcMem, oldPen);
	DeleteObject(hPen);
}
