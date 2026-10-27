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
	if (GetClientRect(hWnd, &rect))
	{
		POINT center;
		center.x = (rect.right - rect.left) / 2;
		center.y = (rect.bottom - rect.top) / 2;
		ClientToScreen(hWnd, &center);
		SetCursorPos(center.x, center.y);

		ScreenToClient(hWnd, &center);
		return MousePosition{.X = center.x, .Y = center.y};
	}
	return std::nullopt;
}
