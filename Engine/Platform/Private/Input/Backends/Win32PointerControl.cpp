#include "PCH.h"
#include "Input/Backends/Win32PointerControl.h"

#include <Windows.h>

bool CaptureWin32Mouse(void* windowHandle)
{
	HWND hWnd = static_cast<HWND>(windowHandle);
	if (hWnd == nullptr || GetForegroundWindow() != hWnd)
	{
		return false;
	}

	SetCapture(hWnd);
	return GetCapture() == hWnd;
}

void ReleaseWin32MouseCapture(void* windowHandle)
{
	if (windowHandle != nullptr && GetCapture() == static_cast<HWND>(windowHandle))
	{
		ReleaseCapture();
	}
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
	const bool bClientRectRead = GetClientRect(hWnd, &rect);
	if (!bClientRectRead || IsRectEmpty(&rect))
	{
		return std::nullopt;
	}

	POINT position;
	position.x = (rect.right - rect.left) / 2;
	position.y = (rect.bottom - rect.top) / 2;
	if (!ClientToScreen(hWnd, &position))
	{
		return std::nullopt;
	}
	if (!SetCursorPos(position.x, position.y))
	{
		return std::nullopt;
	}
	if (!GetCursorPos(&position))
	{
		return std::nullopt;
	}
	if (!ScreenToClient(hWnd, &position))
	{
		return std::nullopt;
	}

	return MousePosition{.X = position.x, .Y = position.y};
}
