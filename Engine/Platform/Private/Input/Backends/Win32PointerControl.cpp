#include "PCH.h"
#include "Input/Backends/Win32PointerControl.h"

#include <Windows.h>

bool CaptureWin32ForegroundMouse()
{
	HWND hWnd = GetForegroundWindow();
	if (hWnd)
	{
		SetCapture(hWnd);
		return true;
	}
	return false;
}

void ReleaseWin32MouseCapture()
{
	ReleaseCapture();
}

void HideWin32Cursor()
{
	while (::ShowCursor(FALSE) >= 0)
	{
		continue;
	}
}

void ShowWin32Cursor()
{
	while (::ShowCursor(TRUE) < 0)
	{
		continue;
	}
}

std::optional<MousePosition> CenterWin32Cursor(void* windowHandle)
{
	HWND hWnd = static_cast<HWND>(windowHandle);
	if (!hWnd)
	{
		return std::nullopt;
	}

	RECT rect;
	if (!GetClientRect(hWnd, &rect) || IsRectEmpty(&rect))
	{
		return std::nullopt;
	}

	POINT position;
	position.x = (rect.right - rect.left) / 2;
	position.y = (rect.bottom - rect.top) / 2;
	if (!ClientToScreen(hWnd, &position) || !SetCursorPos(position.x, position.y) || !GetCursorPos(&position) || !ScreenToClient(hWnd, &position))
	{
		return std::nullopt;
	}

	return MousePosition{.X = position.x, .Y = position.y};
}
