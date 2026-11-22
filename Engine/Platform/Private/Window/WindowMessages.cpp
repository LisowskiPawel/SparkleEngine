#include "PCH.h"
#include "Window/WindowNativeState.h"

#include <commctrl.h>

struct CanceledSdlInput final
{
	SDL_Window* Window;
	InputCancellation Scope;
};

static bool SDLCALL KeepPendingSdlEvent(void* userData, SDL_Event* event) noexcept
{
	const auto& canceled = *static_cast<const CanceledSdlInput*>(userData);
	if (SDL_GetWindowFromEvent(event) != canceled.Window)
	{
		return true;
	}
	const bool bPointerEvent = event->type >= SDL_EVENT_MOUSE_MOTION && event->type <= SDL_EVENT_MOUSE_WHEEL;
	const bool bKeyboardEvent = event->type >= SDL_EVENT_KEY_DOWN && event->type <= SDL_EVENT_TEXT_EDITING_CANDIDATES;
	const bool bCancelKeyboard = canceled.Scope == InputCancellation::All && bKeyboardEvent;
	return !bPointerEvent && !bCancelKeyboard;
}

void Window::CancelInput(InputCancellation cancellation) noexcept
{
	++m_native->PointerCancellationEpoch;
	if (cancellation == InputCancellation::All)
	{
		++m_native->CancellationEpoch;
	}
	// Synchronous native cancellation can interrupt SDL's pump before its queued input is delivered.
	// Discard that pending input as well as invalidating the event already borrowed by observers.
	CanceledSdlInput canceled{m_native->Handle, cancellation};
	SDL_FilterEvents(KeepPendingSdlEvent, &canceled);
	OnInputCanceled.Broadcast(cancellation);
}

void Window::PollEvents() noexcept
{
	m_native->Thread.AssertAccess();
	ApplyPendingShowCommand();
	SDL_Event event;
	bool bNativeStateRefreshed = false;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_EVENT_QUIT)
		{
			RequestClose();
			continue;
		}
		if (SDL_GetWindowFromEvent(&event) != m_native->Handle)
		{
			continue;
		}
		// SDL acquires a batch first; publish its native state before the first observer.
		const bool bWindowEvent = event.type >= SDL_EVENT_WINDOW_FIRST && event.type <= SDL_EVENT_WINDOW_LAST;
		if (!bNativeStateRefreshed || bWindowEvent)
		{
			RefreshNativeState();
			bNativeStateRefreshed = true;
		}
		switch (event.type)
		{
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
				RequestClose();
				break;
			case SDL_EVENT_WINDOW_FOCUS_LOST:
			case SDL_EVENT_WINDOW_MINIMIZED:
				CancelInput(InputCancellation::All);
				break;
			case SDL_EVENT_KEY_DOWN:
				if (!event.key.repeat)
				{
					const bool bAltReturn = event.key.key == SDLK_RETURN && (event.key.mod & SDL_KMOD_ALT) != 0;
					if (event.key.key == SDLK_F11 || bAltReturn)
					{
						ToggleFullScreen();
					}
				}
				break;
			default:
				break;
		}
		const SdlWindowEvent borrowed{event, m_native->CancellationEpoch, m_native->PointerCancellationEpoch};
		m_native->Events.Broadcast(borrowed);
	}
}

void Window::WaitForEvent() noexcept
{
	m_native->Thread.AssertAccess();
	// A null destination waits without consuming the wake event; PollEvents delivers it once.
	if (!m_native->CheckOperation(SDL_WaitEvent(nullptr), "SDL event wait"))
	{
		RequestClose();
	}
}

bool SetSdlWindowRelativeMode(Window& window, bool bEnabled) noexcept
{
	auto& native = *window.m_native;
	native.Thread.AssertAccess();
	if (bEnabled && SDL_GetKeyboardFocus() != native.Handle)
	{
		return false;
	}
	const bool bWasChangingMode = native.bChangingRelativeMode;
	native.bChangingRelativeMode = true;
	const bool bSucceeded = SDL_SetWindowRelativeMouseMode(native.Handle, bEnabled);
	native.bChangingRelativeMode = bWasChangingMode;
	return native.CheckOperation(bSucceeded, "SDL relative pointer mode");
}

LRESULT CALLBACK Window::NativeState::ObserveCancellation(HWND window, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR subclassId, DWORD_PTR userData) noexcept
{
	auto& native = *reinterpret_cast<NativeState*>(userData);
	const bool bLostCapture = message == WM_CAPTURECHANGED && reinterpret_cast<HWND>(lParam) != window;
	if (message == WM_CANCELMODE || (bLostCapture && !native.bChangingRelativeMode))
	{
		native.Owner.CancelInput(InputCancellation::Pointer);
	}
	if (message == WM_ENTERMENULOOP || message == WM_ENTERSIZEMOVE)
	{
		native.Owner.CancelInput(InputCancellation::All);
	}
	if (message == WM_NCDESTROY)
	{
		native.bObservingCancellation = false;
		RemoveWindowSubclass(window, ObserveCancellation, subclassId);
	}
	return DefSubclassProc(window, message, wParam, lParam);
}
