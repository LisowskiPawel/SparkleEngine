#include "PCH.h"
#include "Input/InputSystem.h"
#include "Input/InputSystemState.h"
#include "Window/Window.h"

#include <memory>
#include <utility>

InputSystem::InputSystem() :
    m_state(std::make_unique<State>())
{
}

InputSystem::~InputSystem()
{
	m_state->OwnerThread.AssertAccess();
	m_state->WindowMessageHandle.Reset();
	m_state->ReleaseMouse();
	m_state->SetCursorVisibility(true);
}

void InputSystem::BeginFrame()
{
	m_state->OwnerThread.AssertAccess();
	m_state->GlobalState.BeginFrame();
	for (InputState& state : m_state->LayerStates)
	{
		state.BeginFrame();
	}
	m_state->Dispatcher.ClearDeferredEvents();
}

void InputSystem::ProcessDeferredEvents()
{
	m_state->OwnerThread.AssertAccess();
	m_state->Dispatcher.ProcessDeferredEvents();
}

void InputSystem::SubscribeToWindow(Window& window)
{
	m_state->OwnerThread.AssertAccess();
	m_state->CancelInput();
	m_state->WindowMessageHandle.Reset();
	m_state->WindowHandle = window.GetHWND();

	const auto handle = window.OnWindowMessage.Add([this](WindowMessageEvent& event) { m_state->HandleWindowMessage(event); });
	m_state->WindowMessageHandle = ScopedEventHandle(window.OnWindowMessage, handle);
}

EventHandle InputSystem::SubscribeKeyboard(KeyboardCallback callback, InputLayer layer, DispatchMode mode)
{
	m_state->OwnerThread.AssertAccess();
	return m_state->Dispatcher.Subscribe<KeyboardEvent>(std::move(callback), layer, mode);
}

EventHandle InputSystem::SubscribeMouseButton(MouseButtonCallback callback, InputLayer layer, DispatchMode mode)
{
	m_state->OwnerThread.AssertAccess();
	return m_state->Dispatcher.Subscribe<MouseButtonEvent>(std::move(callback), layer, mode);
}

EventHandle InputSystem::SubscribeMouseMove(MouseMoveCallback callback, InputLayer layer, DispatchMode mode)
{
	m_state->OwnerThread.AssertAccess();
	return m_state->Dispatcher.Subscribe<MouseMoveEvent>(std::move(callback), layer, mode);
}

EventHandle InputSystem::SubscribeMouseWheel(MouseWheelCallback callback, InputLayer layer, DispatchMode mode)
{
	m_state->OwnerThread.AssertAccess();
	return m_state->Dispatcher.Subscribe<MouseWheelEvent>(std::move(callback), layer, mode);
}

void InputSystem::Unsubscribe(EventHandle handle)
{
	m_state->OwnerThread.AssertAccess();
	m_state->Dispatcher.Unsubscribe(handle);
}
