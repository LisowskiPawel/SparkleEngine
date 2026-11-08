#include "PCH.h"
#include "Window/Window.h"

MONITORINFO Window::GetCurrentMonitorInfo() const noexcept
{
	MONITORINFO info{};
	info.cbSize = sizeof(MONITORINFO);
	const HMONITOR monitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	GetMonitorInfoW(monitor, &info);
	return info;
}

void Window::EnterFullScreen()
{
	if (!GetWindowPlacement(m_hWnd, &m_windowedPlacement))
	{
		return;
	}

	m_bFullScreen = true;
	SetWindowLongW(m_hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
	SetWindowLongW(m_hWnd, GWL_EXSTYLE, WS_EX_APPWINDOW);
	const RECT bounds = GetCurrentMonitorInfo().rcMonitor;
	SetWindowPos(m_hWnd, HWND_TOP, bounds.left, bounds.top, bounds.right - bounds.left, bounds.bottom - bounds.top, SWP_FRAMECHANGED | SWP_NOACTIVATE);
	ShowWindow(m_hWnd, SW_SHOW);
}

void Window::RestoreWindowedMode()
{
	m_bFullScreen = false;
	SetWindowLongW(m_hWnd, GWL_STYLE, kWindowedStyle | WS_VISIBLE);
	SetWindowLongW(m_hWnd, GWL_EXSTYLE, kWindowedExStyle);
	SetWindowPlacement(m_hWnd, &m_windowedPlacement);
	SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
}

void Window::Minimize() noexcept
{
	if (m_hWnd != nullptr)
	{
		m_pendingShowCommand = SW_MINIMIZE;
	}
}

void Window::Maximize() noexcept
{
	if (m_hWnd != nullptr)
	{
		m_pendingShowCommand = SW_MAXIMIZE;
	}
}

void Window::Restore() noexcept
{
	if (m_hWnd != nullptr)
	{
		m_pendingShowCommand = SW_RESTORE;
	}
}

void Window::BeginDragMove() noexcept
{
	if (m_hWnd == nullptr || IsFullScreen())
	{
		return;
	}
	ReleaseCapture();
	SendMessageW(m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
}

bool Window::HandleWindowShortcut(UINT message, WPARAM key, LPARAM flags)
{
	const bool bToggleKey = message == WM_KEYDOWN && key == VK_F11;
	const bool bAltReturn = message == WM_SYSKEYDOWN && key == VK_RETURN;
	const bool bAltEnter = bAltReturn && (flags & (LPARAM{1} << 29)) != 0;
	if (!bToggleKey && !bAltEnter)
	{
		return false;
	}
	if ((flags & (LPARAM{1} << 30)) == 0)
	{
		ToggleFullScreen();
	}
	return true;
}

UINT Window::GetDpi() const noexcept
{
	const UINT dpi = m_hWnd != nullptr ? GetDpiForWindow(m_hWnd) : 0;
	return dpi != 0 ? dpi : USER_DEFAULT_SCREEN_DPI;
}

float Window::GetDpiScale() const noexcept
{
	return static_cast<float>(GetDpi()) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
}

void Window::HandleDpiChanged(WPARAM wParam, LPARAM lParam)
{
	const auto* suggested = reinterpret_cast<const RECT*>(lParam);
	if (suggested == nullptr)
	{
		return;
	}

	const RECT bounds = IsFullScreen() ? GetCurrentMonitorInfo().rcMonitor : *suggested;
	SetWindowPos(m_hWnd, nullptr, bounds.left, bounds.top, bounds.right - bounds.left, bounds.bottom - bounds.top, SWP_NOACTIVATE | SWP_NOZORDER);
	const float scale = static_cast<float>(LOWORD(wParam)) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
	OnDpiScaleChanged.Broadcast(scale);
}

void Window::OnSizeChanged(WPARAM sizeType, LPARAM dimensions)
{
	m_clientWidth.store(LOWORD(dimensions), std::memory_order_release);
	m_clientHeight.store(HIWORD(dimensions), std::memory_order_release);
	switch (sizeType)
	{
		case SIZE_MINIMIZED:
			m_state = State::Minimized;
			break;
		case SIZE_RESTORED:
			m_state = State::Normal;
			break;
		case SIZE_MAXIMIZED:
			m_state = State::Maximized;
			break;
	}
	OnResized.Broadcast();
}

void Window::ApplyPendingShowCommand() noexcept
{
	if (m_pendingShowCommand == kNoShowCommand)
	{
		return;
	}

	const int command = m_pendingShowCommand;
	m_pendingShowCommand = kNoShowCommand;
	ShowWindow(m_hWnd, command);
}
