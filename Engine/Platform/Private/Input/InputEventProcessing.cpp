#include "PCH.h"
#include "Input/InputSystemState.h"
#include "Input/SdlKeyboardInput.h"
#include "Window/SdlWindowInterop.h"
#include <SDL3/SDL.h>
#include "Window/Window.h"

#include <type_traits>

template <typename TEvent> void InputSystem::State::ProcessEvent(const TEvent& event)
{
	// Native releases queued before an interruption must not repeat its logical releases.
	if constexpr (std::is_same_v<TEvent, KeyboardEvent>)
	{
		if (!event.bPressed && !GlobalState.IsKeyDown(event.KeyCode))
		{
			return;
		}
	}
	if constexpr (std::is_same_v<TEvent, MouseButtonEvent>)
	{
		if (!event.bPressed && !GlobalState.IsMouseButtonDown(event.Button))
		{
			return;
		}
	}
	InputLayer targetLayer = MouseCaptureLayer ? *MouseCaptureLayer : FocusRouter.ResolveTargetLayer(event);
	if constexpr (std::is_same_v<TEvent, MouseButtonEvent>)
	{
		if (event.bPressed)
		{
			const auto cancellationEpoch = CancellationEpoch;
			SetActiveLayer(targetLayer);
			if (cancellationEpoch != CancellationEpoch)
			{
				return;
			}

			// Focus cancellation can reenter routing, for example by opening a modal.
			targetLayer = MouseCaptureLayer ? *MouseCaptureLayer : FocusRouter.ResolveTargetLayer(event);
		}
	}

	UpdateStateFromEvent(GlobalState, event);
	InputState& targetState = GetRoutedState(targetLayer);
	UpdateStateFromEvent(targetState, event);

	// Queue before user code so nested cancellation removes this event as well.
	Dispatcher.QueueDeferred(event, targetLayer);
	Dispatcher.DispatchImmediate(event, targetLayer);
}

void InputSystem::State::HandleWindowEvent(const SdlWindowEvent& message)
{
	if (BoundWindow->ShouldClose() || !IsSdlWindowEventCurrent(*BoundWindow, message))
	{
		return;
	}
	const SDL_Event& event = message.Value;
	switch (event.type)
	{
		case SDL_EVENT_WINDOW_FOCUS_GAINED:
			GlobalState.SetModifiers(TranslateSdlModifiers(SDL_GetModState()));
			break;
		case SDL_EVENT_KEY_DOWN:
		case SDL_EVENT_KEY_UP:
		{
			const KeyboardEvent key{.KeyCode = TranslateSdlKey(event.key.key), .Modifiers = TranslateSdlModifiers(event.key.mod), .bPressed = event.key.down, .bRepeat = event.key.repeat};
			ProcessEvent(key);
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			MouseButton button;
			switch (event.button.button)
			{
				case SDL_BUTTON_LEFT:
					button = MouseButton::Left;
					break;
				case SDL_BUTTON_MIDDLE:
					button = MouseButton::Middle;
					break;
				case SDL_BUTTON_RIGHT:
					button = MouseButton::Right;
					break;
				default:
					return;
			}
			// SDL's pump may already have acquired later keys; retain the preceding key event's modifiers.
			const MouseButtonEvent mouse{.Button = button, .Position = {event.button.x, event.button.y}, .Modifiers = GlobalState.GetModifiers(), .bPressed = event.button.down};
			ProcessEvent(mouse);
			break;
		}
		case SDL_EVENT_MOUSE_MOTION:
			ProcessEvent(MouseMoveEvent{.Position = {event.motion.x, event.motion.y}, .Delta = {event.motion.xrel, event.motion.yrel}, .Modifiers = GlobalState.GetModifiers()});
			break;
		case SDL_EVENT_MOUSE_WHEEL:
		{
			const float direction = event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1.0f : 1.0f;
			const MousePosition position{event.wheel.mouse_x, event.wheel.mouse_y};
			if (event.wheel.y != 0.0f)
			{
				ProcessEvent(MouseWheelEvent{event.wheel.y * direction, position, false});
			}
			if (event.wheel.x != 0.0f && IsSdlWindowEventCurrent(*BoundWindow, message))
			{
				ProcessEvent(MouseWheelEvent{event.wheel.x * direction, position, true});
			}
			break;
		}
		default:
			break;
	}
}

void InputSystem::State::UpdateStateFromEvent(InputState& state, const KeyboardEvent& event)
{
	if (!event.bRepeat || !state.IsKeyDown(event.KeyCode))
	{
		state.SetKeyState(event.KeyCode, event.bPressed ? ButtonState::Pressed : ButtonState::Released);
	}
	state.SetModifiers(event.Modifiers);
}

void InputSystem::State::UpdateStateFromEvent(InputState& state, const MouseButtonEvent& event)
{
	state.SetMouseButtonState(event.Button, event.bPressed ? ButtonState::Pressed : ButtonState::Released);
	state.SetMousePosition(event.Position.X, event.Position.Y);
	state.SetModifiers(event.Modifiers);
}

void InputSystem::State::UpdateStateFromEvent(InputState& state, const MouseMoveEvent& event)
{
	state.SetMousePosition(event.Position.X, event.Position.Y);
	state.AccumulateMouseDelta(event.Delta.X, event.Delta.Y);
	state.SetModifiers(event.Modifiers);
}

void InputSystem::State::UpdateStateFromEvent(InputState& state, const MouseWheelEvent& event)
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
