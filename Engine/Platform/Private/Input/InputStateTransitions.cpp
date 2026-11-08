#include "PCH.h"
#include "Input/InputSystem.h"
#include "Input/InputDispatchState.h"
#include "Input/InputBackendResult.h"

void InputSystem::AdvanceFrameState()
{
	m_inputState.BeginFrame();
	for (InputState& state : m_layerStates)
	{
		state.BeginFrame();
	}
}

void InputSystem::UpdateMouseMoveDelta(MouseMoveEvent& event) noexcept
{
	event.Delta.X = m_bHasLastMousePosition ? event.Position.X - m_lastMouseX : 0;
	event.Delta.Y = m_bHasLastMousePosition ? event.Position.Y - m_lastMouseY : 0;
	m_lastMouseX = event.Position.X;
	m_lastMouseY = event.Position.Y;
	m_bHasLastMousePosition = true;
}

template <typename TEvent> void InputSystem::ProcessEvent(const TEvent& event, InputLayer targetLayer)
{
	UpdateStateFromEvent(m_inputState, event);
	if (InputState* state = FindLayerState(targetLayer))
	{
		UpdateStateFromEvent(*state, event);
	}

	// Queue before user code so a nested cancellation removes this event as well.
	m_dispatch->EventDispatcher.QueueDeferred(event, targetLayer, m_layerEnabled);
	m_dispatch->EventDispatcher.DispatchImmediate(event, targetLayer, m_layerEnabled);
}

void InputSystem::DispatchInputEvent(const InputBackendResult& result, InputLayer targetLayer)
{
	switch (result.Type)
	{
		case InputEventType::Keyboard:
			ProcessEvent(result.Keyboard, targetLayer);
			break;
		case InputEventType::MouseButton:
			ProcessEvent(result.MouseButton, targetLayer);
			break;
		case InputEventType::MouseMove:
			ProcessEvent(result.MouseMove, targetLayer);
			break;
		case InputEventType::MouseWheel:
			ProcessEvent(result.MouseWheel, targetLayer);
			break;
		default:
			break;
	}
}

void InputSystem::UpdateStateFromEvent(InputState& state, const KeyboardEvent& event)
{
	if (!event.bRepeat || !state.IsKeyDown(event.KeyCode))
	{
		state.SetKeyState(event.KeyCode, event.bPressed ? ButtonState::Pressed : ButtonState::Released);
	}
	state.SetModifiers(event.Modifiers);
}

void InputSystem::UpdateStateFromEvent(InputState& state, const MouseButtonEvent& event)
{
	state.SetMouseButtonState(event.Button, event.bPressed ? ButtonState::Pressed : ButtonState::Released);
	state.SetMousePosition(event.Position.X, event.Position.Y);
	state.SetModifiers(event.Modifiers);
}

void InputSystem::UpdateStateFromEvent(InputState& state, const MouseMoveEvent& event)
{
	state.SetMousePosition(event.Position.X, event.Position.Y);
	state.AccumulateMouseDelta(event.Delta.X, event.Delta.Y);
	state.SetModifiers(event.Modifiers);
}

void InputSystem::UpdateStateFromEvent(InputState& state, const MouseWheelEvent& event)
{
	if (event.bHorizontal)
	{
		state.AccumulateWheelHorizontalDelta(event.Delta);
	}
	else
	{
		state.AccumulateWheelDelta(event.Delta);
	}
	state.SetMousePosition(event.Position.X, event.Position.Y);
}
