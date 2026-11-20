#include "PCH.h"
#include "Input/InputSystemState.h"
#include "Input/Backends/Win32InputBackend.h"
#include "Window/Window.h"

#include <type_traits>
#include <variant>

void InputSystem::State::UpdateMouseMoveDelta(MouseMoveEvent& event) noexcept
{
	event.Delta.X = bHasLastMousePosition ? event.Position.X - LastMouseX : 0;
	event.Delta.Y = bHasLastMousePosition ? event.Position.Y - LastMouseY : 0;
	LastMouseX = event.Position.X;
	LastMouseY = event.Position.Y;
	bHasLastMousePosition = true;
}

template <typename TEvent> void InputSystem::State::ProcessEvent(const TEvent& event)
{
	InputLayer targetLayer = GlobalState.IsMouseCaptured() ? MouseCaptureLayer : FocusRouter.ResolveTargetLayer(event);
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
			targetLayer = GlobalState.IsMouseCaptured() ? MouseCaptureLayer : FocusRouter.ResolveTargetLayer(event);
		}
	}

	UpdateStateFromEvent(GlobalState, event);
	InputState& targetState = GetRoutedState(targetLayer);
	UpdateStateFromEvent(targetState, event);

	// Queue before user code so nested cancellation removes this event as well.
	Dispatcher.QueueDeferred(event, targetLayer);
	Dispatcher.DispatchImmediate(event, targetLayer);
}

void InputSystem::State::HandleWindowMessage(const WindowMessageEvent& message)
{
	Win32InputEvent input = ProcessWin32InputMessage(message);
	if (auto* move = std::get_if<MouseMoveEvent>(&input))
	{
		UpdateMouseMoveDelta(*move);
	}
	std::visit([this](const auto& event) { ProcessEvent(event); }, input);
}

void InputSystem::State::ProcessEvent(const std::monostate&) noexcept
{
}

void InputSystem::State::ProcessEvent(const InputFocusLost&)
{
	CancelInput();
}

void InputSystem::State::ProcessEvent(const InputCaptureLost&)
{
	CancelPointerInput();
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
