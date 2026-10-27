#include "PCH.h"
#include "Window/Window.h"

#include "Core/Public/Diagnostics/Logger.h"
#include "Core/Public/Diagnostics/Verify.h"

#include <Windows.h>

#include <atomic>
#include <cstdint>
#include <dwmapi.h>
#include <string>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_platformLogger, "Platform");

Window::Window(std::string_view windowTitle)
{
	m_hInstance = GetModuleHandleW(nullptr);

	RegisterWindowClass();
	CreateWindowHandle(windowTitle);
	ApplyInitialWindowState();
}

Window::~Window()
{
	if (m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = nullptr;
	}

	if (m_windowClassAtom && m_hInstance)
	{
		UnregisterClassW(kWindowClassName, m_hInstance);
		m_windowClassAtom = 0;
	}
}

void Window::RegisterWindowClass()
{
	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = &Window::WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(Window*);
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIconW(nullptr, MAKEINTRESOURCEW(32512));
	wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = kWindowClassName;
	wc.hIconSm = LoadIconW(nullptr, MAKEINTRESOURCEW(32512));

	m_windowClassAtom = RegisterClassExW(&wc);
	if (!m_windowClassAtom)
	{
		Diagnostics::Fatal(g_platformLogger, __FILE__, __LINE__, "Window: Failed to register window class");
	}
}

void Window::CreateWindowHandle(std::string_view title)
{
	const std::wstring wideTitle(title.begin(), title.end());

	m_hWnd = CreateWindowExW(kWindowedExStyle, kWindowClassName, wideTitle.c_str(), kWindowedStyle, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, m_hInstance, this);

	if (!m_hWnd)
	{
		Diagnostics::Fatal(g_platformLogger, __FILE__, __LINE__, "Window: Failed to create window");
	}

	const MARGINS margins{-1, -1, -1, -1};
	DwmExtendFrameIntoClientArea(m_hWnd, &margins);
}

void Window::ApplyInitialWindowState()
{
	GetWindowRect(m_hWnd, &m_windowedRect);
	ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);
}

void Window::PollEvents() noexcept
{
	ApplyPendingShowCommand();

	MSG msg{};
	while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}

void Window::WaitForEvent() noexcept
{
	if (m_hWnd == nullptr || m_bShouldClose)
	{
		return;
	}

	WaitMessage();
}

MONITORINFO Window::GetCurrentMonitorInfo() const noexcept
{
	MONITORINFO monitorInfo{};
	monitorInfo.cbSize = sizeof(MONITORINFO);
	HMONITOR hMonitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	GetMonitorInfoW(hMonitor, &monitorInfo);
	return monitorInfo;
}

void Window::SetFullScreen(bool bFullScreen)
{
	if (IsFullScreen() == bFullScreen)
	{
		return;
	}

	if (bFullScreen)
	{
		EnterFullScreen();
	}
	else
	{
		RestoreWindowedMode();
	}
}

void Window::EnterFullScreen()
{
	GetWindowRect(m_hWnd, &m_windowedRect);

	SetWindowLongW(m_hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
	SetWindowLongW(m_hWnd, GWL_EXSTYLE, WS_EX_APPWINDOW);

	m_state = State::FullScreen;

	const RECT rc = GetCurrentMonitorInfo().rcMonitor;
	SetWindowPos(m_hWnd, HWND_TOP, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_FRAMECHANGED | SWP_NOACTIVATE);

	ShowWindow(m_hWnd, SW_SHOW);
}

void Window::RestoreWindowedMode()
{
	SetWindowLongW(m_hWnd, GWL_STYLE, kWindowedStyle | WS_VISIBLE);
	SetWindowLongW(m_hWnd, GWL_EXSTYLE, kWindowedExStyle);

	SetWindowPos(
	    m_hWnd,
	    HWND_NOTOPMOST,
	    m_windowedRect.left,
	    m_windowedRect.top,
	    m_windowedRect.right - m_windowedRect.left,
	    m_windowedRect.bottom - m_windowedRect.top,
	    SWP_FRAMECHANGED | SWP_NOACTIVATE);

	m_state = State::Normal;
	ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);
}

bool Window::HasValidSize() const noexcept
{
	return GetWidth() > 0 && GetHeight() > 0;
}

float Window::GetDpiScale() const noexcept
{
	return static_cast<float>(GetDpi()) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
}

void Window::ToggleFullScreen()
{
	SetFullScreen(!IsFullScreen());
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

void Window::ToggleMaximizeRestore() noexcept
{
	if (m_hWnd == nullptr || IsFullScreen())
	{
		return;
	}

	if (IsMaximized())
	{
		Restore();
	}
	else
	{
		Maximize();
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

LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* window = nullptr;

	if (msg == WM_NCCREATE)
	{
		auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
		window = static_cast<Window*>(create->lpCreateParams);
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));

		window->m_hWnd = hWnd;
	}
	else
	{
		window = reinterpret_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
	}

	if (window)
	{
		return window->HandleMessage(msg, wParam, lParam);
	}

	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	WindowMessageEvent msgEvent{m_hWnd, msg, wParam, lParam, false};
	OnWindowMessage.Broadcast(msgEvent);

	if (msgEvent.handled)
	{
		return 0;
	}

	switch (msg)
	{
		case WM_NCCALCSIZE:
			return HandleNonClientSize(wParam, lParam);

		case WM_NCHITTEST:
			return HandleResizeHitTest(wParam, lParam);

		case WM_SIZE:
			OnSizeChanged(wParam, LOWORD(lParam), HIWORD(lParam));
			return 0;

		case WM_DPICHANGED:
		{
			const auto* suggestedWindowRect = reinterpret_cast<const RECT*>(lParam);
			if (suggestedWindowRect != nullptr)
			{
				HandleDpiChanged(LOWORD(wParam), *suggestedWindowRect);
			}
			return 0;
		}

		case WM_CLOSE:
			m_bShouldClose = true;
			return 0;

		case WM_DESTROY:
			m_hWnd = nullptr;
			return 0;

		case WM_KEYDOWN:

			if (wParam == VK_F11)
			{
				ToggleFullScreen();
				return 0;
			}
			break;

		case WM_SYSKEYDOWN:

			if (wParam == VK_RETURN && (lParam & (1 << 29)))
			{
				ToggleFullScreen();
				return 0;
			}
			break;

		case WM_GETMINMAXINFO:
			ApplyTrackSizeConstraints(*reinterpret_cast<MINMAXINFO*>(lParam));
			return 0;

		case WM_ERASEBKGND:
			return 1;
	}

	return DefWindowProcW(m_hWnd, msg, wParam, lParam);
}

UINT Window::GetDpi() const noexcept
{
	const UINT dpi = m_hWnd != nullptr ? GetDpiForWindow(m_hWnd) : 0;
	return dpi != 0 ? dpi : USER_DEFAULT_SCREEN_DPI;
}

void Window::HandleDpiChanged(UINT dpi, const RECT& suggestedWindowRect)
{
	RECT targetRect = suggestedWindowRect;
	if (IsFullScreen())
	{
		targetRect = GetCurrentMonitorInfo().rcMonitor;
	}

	SetWindowPos(m_hWnd, nullptr, targetRect.left, targetRect.top, targetRect.right - targetRect.left, targetRect.bottom - targetRect.top, SWP_NOACTIVATE | SWP_NOZORDER);

	const float dpiScale = static_cast<float>(dpi) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
	OnDpiScaleChanged.Broadcast(dpiScale);
}

void Window::OnSizeChanged(WPARAM sizeType, uint32_t width, uint32_t height)
{
	m_clientWidth.store(width, std::memory_order_release);
	m_clientHeight.store(height, std::memory_order_release);

	if (m_state != State::FullScreen)
	{
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
