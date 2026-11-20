#include "PCH.h"
#include "Window/Window.h"

#include <optional>

static std::optional<LRESULT> ClassifyResizeBorder(const POINT& cursorPoint, const RECT& windowRect, int border) noexcept
{
	const bool bOnLeft = cursorPoint.x >= windowRect.left && cursorPoint.x < windowRect.left + border;
	const bool bOnRight = cursorPoint.x <= windowRect.right && cursorPoint.x > windowRect.right - border;
	const bool bOnTop = cursorPoint.y >= windowRect.top && cursorPoint.y < windowRect.top + border;
	const bool bOnBottom = cursorPoint.y <= windowRect.bottom && cursorPoint.y > windowRect.bottom - border;

	if (bOnTop && bOnLeft)
	{
		return HTTOPLEFT;
	}

	if (bOnTop && bOnRight)
	{
		return HTTOPRIGHT;
	}

	if (bOnBottom && bOnLeft)
	{
		return HTBOTTOMLEFT;
	}

	if (bOnBottom && bOnRight)
	{
		return HTBOTTOMRIGHT;
	}

	if (bOnLeft)
	{
		return HTLEFT;
	}

	if (bOnRight)
	{
		return HTRIGHT;
	}

	if (bOnTop)
	{
		return HTTOP;
	}

	if (bOnBottom)
	{
		return HTBOTTOM;
	}

	return std::nullopt;
}

LRESULT Window::HandleNonClientSize(WPARAM wParam, LPARAM lParam) const
{
	if (wParam == TRUE && IsZoomed(m_hWnd))
	{
		auto* params = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
		params->rgrc[0] = GetCurrentMonitorInfo().rcWork;
	}

	return 0;
}

LRESULT Window::HandleResizeHitTest(WPARAM wParam, LPARAM lParam) const
{
	if (IsFullScreen() || IsZoomed(m_hWnd))
	{
		return DefWindowProcW(m_hWnd, WM_NCHITTEST, wParam, lParam);
	}

	const short cursorX = static_cast<short>(LOWORD(lParam));
	const short cursorY = static_cast<short>(HIWORD(lParam));
	const POINT cursorPoint{cursorX, cursorY};
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

LRESULT Window::HandleTrackSizeConstraints(LPARAM lParam) const
{
	auto& minMaxInfo = *reinterpret_cast<MINMAXINFO*>(lParam);
	const int dpi = static_cast<int>(GetDpi());
	minMaxInfo.ptMinTrackSize.x = MulDiv(kMinWindowWidth, dpi, USER_DEFAULT_SCREEN_DPI);
	minMaxInfo.ptMinTrackSize.y = MulDiv(kMinWindowHeight, dpi, USER_DEFAULT_SCREEN_DPI);

	const MONITORINFO monitorInfo = GetCurrentMonitorInfo();
	const RECT& bounds = IsFullScreen() ? monitorInfo.rcMonitor : monitorInfo.rcWork;
	minMaxInfo.ptMaxPosition.x = bounds.left - monitorInfo.rcMonitor.left;
	minMaxInfo.ptMaxPosition.y = bounds.top - monitorInfo.rcMonitor.top;
	minMaxInfo.ptMaxSize.x = bounds.right - bounds.left;
	minMaxInfo.ptMaxSize.y = bounds.bottom - bounds.top;
	return 0;
}

int Window::GetResizeBorderThickness() const noexcept
{
	const UINT dpi = GetDpi();
	const int frameWidth = GetSystemMetricsForDpi(SM_CXFRAME, dpi);
	const int framePadding = GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
	return frameWidth + framePadding;
}
