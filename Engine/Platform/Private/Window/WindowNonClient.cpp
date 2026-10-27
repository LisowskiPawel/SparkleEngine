#include "PCH.h"
#include "Window/Window.h"

#include <optional>

static std::optional<LRESULT> ClassifyResizeBorder(const POINT& cursorPoint, const RECT& windowRect, int border) noexcept
{
	const bool onLeft = cursorPoint.x >= windowRect.left && cursorPoint.x < windowRect.left + border;
	const bool onRight = cursorPoint.x <= windowRect.right && cursorPoint.x > windowRect.right - border;
	const bool onTop = cursorPoint.y >= windowRect.top && cursorPoint.y < windowRect.top + border;
	const bool onBottom = cursorPoint.y <= windowRect.bottom && cursorPoint.y > windowRect.bottom - border;

	if (onTop && onLeft)
	{
		return HTTOPLEFT;
	}

	if (onTop && onRight)
	{
		return HTTOPRIGHT;
	}

	if (onBottom && onLeft)
	{
		return HTBOTTOMLEFT;
	}

	if (onBottom && onRight)
	{
		return HTBOTTOMRIGHT;
	}

	if (onLeft)
	{
		return HTLEFT;
	}

	if (onRight)
	{
		return HTRIGHT;
	}

	if (onTop)
	{
		return HTTOP;
	}

	if (onBottom)
	{
		return HTBOTTOM;
	}

	return std::nullopt;
}

LRESULT Window::HandleNonClientSize(WPARAM wParam, LPARAM lParam) const
{
	if (wParam == TRUE)
	{
		if (IsZoomed(m_hWnd))
		{
			auto* params = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
			params->rgrc[0] = GetCurrentMonitorInfo().rcWork;
		}
		return 0;
	}

	return 0;
}

LRESULT Window::HandleResizeHitTest(WPARAM wParam, LPARAM lParam) const
{
	if (IsFullScreen() || IsZoomed(m_hWnd))
	{
		return DefWindowProcW(m_hWnd, WM_NCHITTEST, wParam, lParam);
	}

	POINT cursorPoint{static_cast<LONG>(static_cast<short>(LOWORD(lParam))), static_cast<LONG>(static_cast<short>(HIWORD(lParam)))};
	RECT windowRect{};
	if (!GetWindowRect(m_hWnd, &windowRect))
	{
		return DefWindowProcW(m_hWnd, WM_NCHITTEST, wParam, lParam);
	}

	const int border = GetResizeBorderThickness();
	const std::optional<LRESULT> borderHit = ClassifyResizeBorder(cursorPoint, windowRect, border);
	if (borderHit)
	{
		return *borderHit;
	}

	return DefWindowProcW(m_hWnd, WM_NCHITTEST, wParam, lParam);
}

void Window::ApplyTrackSizeConstraints(MINMAXINFO& minMaxInfo) const
{
	const UINT dpi = GetDpi();
	minMaxInfo.ptMinTrackSize.x = MulDiv(kMinWindowWidth, static_cast<int>(dpi), USER_DEFAULT_SCREEN_DPI);
	minMaxInfo.ptMinTrackSize.y = MulDiv(kMinWindowHeight, static_cast<int>(dpi), USER_DEFAULT_SCREEN_DPI);
	const MONITORINFO mi = GetCurrentMonitorInfo();
	minMaxInfo.ptMaxPosition.x = mi.rcWork.left - mi.rcMonitor.left;
	minMaxInfo.ptMaxPosition.y = mi.rcWork.top - mi.rcMonitor.top;
	minMaxInfo.ptMaxSize.x = mi.rcWork.right - mi.rcWork.left;
	minMaxInfo.ptMaxSize.y = mi.rcWork.bottom - mi.rcWork.top;
}

int Window::GetResizeBorderThickness() const noexcept
{
	const UINT dpi = GetDpi();
	return GetSystemMetricsForDpi(SM_CXFRAME, dpi) + GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
}
