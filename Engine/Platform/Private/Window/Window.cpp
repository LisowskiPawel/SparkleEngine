#include "PCH.h"
#include "Window/Window.h"

Window::Window(std::string_view windowTitle)
{
	InitializeNativeWindow(windowTitle);
	ApplyInitialWindowState();
}

Window::~Window()
{
	DestroyNativeWindow();
}

void Window::PollEvents() noexcept
{
	ApplyPendingShowCommand();
	PumpNativeMessages();
}

bool Window::HasValidSize() const noexcept
{
	return GetWidth() > 0 && GetHeight() > 0;
}

Window::State Window::GetState() const noexcept
{
	if (IsMinimized())
	{
		return State::Minimized;
	}
	return IsFullScreen() ? State::FullScreen : m_state;
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

void Window::ToggleFullScreen()
{
	SetFullScreen(!IsFullScreen());
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

LRESULT Window::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	WindowMessageEvent event{m_hWnd, msg, wParam, lParam, false};
	OnWindowMessage.Broadcast(event);
	if (event.bHandled)
	{
		return 0;
	}

	switch (msg)
	{
		case WM_NCCALCSIZE:
			return HandleNonClientSize(wParam, lParam);
		case WM_NCHITTEST:
			return HandleResizeHitTest(wParam, lParam);
		case WM_GETMINMAXINFO:
			return HandleTrackSizeConstraints(lParam);
		case WM_SIZE:
			OnSizeChanged(wParam, lParam);
			return 0;
		case WM_DPICHANGED:
			HandleDpiChanged(wParam, lParam);
			return 0;
		case WM_CLOSE:
		case WM_DESTROY:
			RequestClose();
			return 0;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			if (HandleWindowShortcut(msg, wParam, lParam))
			{
				return 0;
			}
			break;
		case WM_ERASEBKGND:
			return 1;
	}

	return DefWindowProcW(event.hWnd, msg, wParam, lParam);
}
