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

bool Window::IsMaximized() const noexcept
{
	return GetState() == State::Maximized;
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
