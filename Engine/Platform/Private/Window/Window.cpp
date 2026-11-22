#include "PCH.h"
#include "Window/WindowNativeState.h"
#include "Window/Win32WindowInterop.h"

Window::Window(std::string_view windowTitle) :
    m_native(std::make_unique<NativeState>(*this))
{
	m_native->Initialize(windowTitle);
	RefreshNativeState();
}

Window::~Window()
{
	// Detach native callbacks while all of the Window's published state is still alive.
	m_native->Shutdown();
}

bool Window::HasValidSize() const noexcept
{
	return GetWidth() > 0 && GetHeight() > 0;
}

void* GetWin32WindowHandle(const Window& window) noexcept
{
	return window.m_native->NativeHandle;
}

SDL_Window* GetSdlWindow(const Window& window) noexcept
{
	window.m_native->Thread.AssertAccess();
	return window.m_native->Handle;
}

Event<void(const SdlWindowEvent&), 16>& GetSdlWindowEvents(Window& window) noexcept
{
	window.m_native->Thread.AssertAccess();
	return window.m_native->Events;
}

bool IsSdlWindowEventCurrent(const Window& window, const SdlWindowEvent& event) noexcept
{
	window.m_native->Thread.AssertAccess();
	const bool bPointerEvent = event.Value.type >= SDL_EVENT_MOUSE_MOTION && event.Value.type <= SDL_EVENT_MOUSE_WHEEL;
	const bool bPointerCurrent = !bPointerEvent || window.m_native->PointerCancellationEpoch == event.PointerCancellationEpoch;
	return window.m_native->CancellationEpoch == event.CancellationEpoch && bPointerCurrent;
}

void Window::RequestClose() noexcept
{
	m_native->Thread.AssertAccess();
	if (!m_bShouldClose)
	{
		m_bShouldClose = true;
		CancelInput(InputCancellation::All);
	}
}
