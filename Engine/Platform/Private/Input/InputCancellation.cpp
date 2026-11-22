#include "PCH.h"
#include "Input/InputSystemState.h"
#include "Window/Window.h"

#include <utility>

void InputSystem::State::CancelInput()
{
	++CancellationEpoch;

	// These snapshots outlive publication of empty state while cancellation callbacks run.
	InputState canceledGlobal = std::move(GlobalState);
	auto canceledLayers = std::move(LayerStates);
	const auto capturedLayer = std::exchange(MouseCaptureLayer, std::nullopt);

	GlobalState = InputState{};
	LayerStates = {};
	FocusRouter.ResetFocus();
	Dispatcher.CancelAllEvents();

	if (capturedLayer)
	{
		DisableNativeRelativeMode();
	}

	for (std::size_t index = 0; index < RoutedLayerCount; ++index)
	{
		const auto layer = static_cast<InputLayer>(index + 1);
		ReleaseStateKeys(canceledLayers[index], layer);
		ReleaseStateMouseButtons(canceledLayers[index], layer);
	}

	ReleaseStateKeys(canceledGlobal, InputLayer::System);
	ReleaseStateMouseButtons(canceledGlobal, InputLayer::System);
}

void InputSystem::State::CancelPointerInput()
{
	++CancellationEpoch;
	InputState canceledGlobal = GlobalState.ExtractPointerState();
	std::array<InputState, RoutedLayerCount> canceledLayers;
	for (std::size_t index = 0; index < RoutedLayerCount; ++index)
	{
		canceledLayers[index] = LayerStates[index].ExtractPointerState();
	}

	const auto capturedLayer = std::exchange(MouseCaptureLayer, std::nullopt);
	Dispatcher.CancelPointerEvents();

	if (capturedLayer)
	{
		DisableNativeRelativeMode();
	}

	for (std::size_t index = 0; index < RoutedLayerCount; ++index)
	{
		ReleaseStateMouseButtons(canceledLayers[index], static_cast<InputLayer>(index + 1));
	}

	ReleaseStateMouseButtons(canceledGlobal, InputLayer::System);
}

void InputSystem::State::CancelLayer(InputLayer layer)
{
	InputState& state = GetRoutedState(layer);
	InputState canceled = std::move(state);
	state = InputState{};
	Dispatcher.CancelLayerEvents(layer);
	if (MouseCaptureLayer == layer)
	{
		ReleaseRelativeMode();
	}

	ReleaseStateKeys(canceled, layer);
	ReleaseStateMouseButtons(canceled, layer);
}

void InputSystem::State::ReleaseStateKeys(const InputState& state, InputLayer layer)
{
	for (std::size_t index = 0; index < static_cast<std::size_t>(Key::Count); ++index)
	{
		const auto key = static_cast<Key>(index);
		if (state.IsKeyDown(key))
		{
			KeyboardEvent event{};
			event.KeyCode = key;
			event.Modifiers = state.GetModifiers();
			Dispatcher.DispatchCancellation(event, layer);
		}
	}
}

void InputSystem::State::ReleaseStateMouseButtons(const InputState& state, InputLayer layer)
{
	for (std::size_t index = 0; index < static_cast<std::size_t>(MouseButton::Count); ++index)
	{
		const auto button = static_cast<MouseButton>(index);
		if (state.IsMouseButtonDown(button))
		{
			MouseButtonEvent event{};
			event.Button = button;
			event.Position = state.GetMousePosition();
			event.Modifiers = state.GetModifiers();
			Dispatcher.DispatchCancellation(event, layer);
		}
	}
}
