#include "PCH.h"
#include "Input/InputSystem.h"
#include "Input/InputSystemState.h"
#include "Input/SdlKeyboardInput.h"
#include <SDL3/SDL_keyboard.h>
#include "Window/Window.h"
#include "Window/SdlWindowInterop.h"

#include <memory>
#include <utility>

InputSystem::InputSystem() :
    m_state(std::make_unique<State>())
{
}

InputSystem::~InputSystem()
{
	m_state->WindowEventHandle.Reset();
	m_state->WindowCancellationHandle.Reset();
	m_state->ReleaseRelativeMode();
}

void InputSystem::BeginFrame()
{
	m_state->GlobalState.BeginFrame();
	for (InputState& state : m_state->LayerStates)
	{
		state.BeginFrame();
	}
	m_state->Dispatcher.ClearDeferredEvents();
}

void InputSystem::ProcessDeferredEvents()
{
	m_state->Dispatcher.ProcessDeferredEvents();
}

void InputSystem::SubscribeToWindow(Window& window)
{
	m_state->CancelInput();
	m_state->WindowEventHandle.Reset();
	m_state->WindowCancellationHandle.Reset();
	m_state->BoundWindow = &window;
	m_state->GlobalState.SetModifiers(TranslateSdlModifiers(SDL_GetModState()));
	auto& events = GetSdlWindowEvents(window);
	const auto eventHandle = events.Add([this](const SdlWindowEvent& event) { m_state->HandleWindowEvent(event); });
	m_state->WindowEventHandle = ScopedEventHandle(events, eventHandle);
	const auto cancelHandle = window.OnInputCanceled.Add(
	    [this](InputCancellation cancellation)
	    {
		    if (cancellation == InputCancellation::All)
		    {
			    m_state->CancelInput();
		    }
		    else
		    {
			    m_state->CancelPointerInput();
		    }
	    });
	m_state->WindowCancellationHandle = ScopedEventHandle(window.OnInputCanceled, cancelHandle);
}

EventHandle InputSystem::SubscribeKeyboard(KeyboardCallback callback, InputLayer layer, DispatchMode mode)
{
	return m_state->Dispatcher.Subscribe<KeyboardEvent>(std::move(callback), layer, mode);
}

EventHandle InputSystem::SubscribeMouseButton(MouseButtonCallback callback, InputLayer layer, DispatchMode mode)
{
	return m_state->Dispatcher.Subscribe<MouseButtonEvent>(std::move(callback), layer, mode);
}

EventHandle InputSystem::SubscribeMouseMove(MouseMoveCallback callback, InputLayer layer, DispatchMode mode)
{
	return m_state->Dispatcher.Subscribe<MouseMoveEvent>(std::move(callback), layer, mode);
}

EventHandle InputSystem::SubscribeMouseWheel(MouseWheelCallback callback, InputLayer layer, DispatchMode mode)
{
	return m_state->Dispatcher.Subscribe<MouseWheelEvent>(std::move(callback), layer, mode);
}

void InputSystem::Unsubscribe(EventHandle handle)
{
	m_state->Dispatcher.Unsubscribe(handle);
}
