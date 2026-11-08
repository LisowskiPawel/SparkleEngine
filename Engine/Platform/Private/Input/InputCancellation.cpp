#include "PCH.h"
#include "Input/InputSystem.h"
#include "Input/InputDispatchState.h"
#include "Input/Backends/Win32PointerControl.h"

#include <utility>

void InputSystem::CancelInput()
{
	// These snapshots outlive publication of empty state while cancellation callbacks run.
	InputState canceledGlobal = std::move(m_inputState);
	auto canceledLayers = std::move(m_layerStates);
	m_inputState = InputState{};
	m_layerStates = {};
	m_mouseCaptureLayer = InputLayer::System;
	m_bHasLastMousePosition = false;
	m_dispatch->FocusRouter.ResetFocus();
	m_dispatch->EventDispatcher.CancelAllEvents();

	if (canceledGlobal.IsMouseCaptured())
	{
		ReleaseWin32MouseCapture(m_windowHandle);
	}
	if (canceledGlobal.IsCursorHidden())
	{
		ShowWin32Cursor();
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

InputState InputSystem::TakePointerState(InputState& state) noexcept
{
	InputState canceled;
	for (std::size_t index = 0; index < static_cast<std::size_t>(MouseButton::Count); ++index)
	{
		const auto button = static_cast<MouseButton>(index);
		canceled.SetMouseButtonState(button, state.GetMouseButtonState(button));
	}
	const MousePosition position = state.GetMousePosition();
	canceled.SetMousePosition(position.X, position.Y);
	canceled.SetModifiers(state.GetModifiers());
	canceled.SetMouseCaptured(state.IsMouseCaptured());
	canceled.SetCursorHidden(state.IsCursorHidden());

	state.ResetPointerState();
	return canceled;
}

void InputSystem::CancelPointerInput()
{
	InputState canceledGlobal = TakePointerState(m_inputState);
	std::array<InputState, RoutedLayerCount> canceledLayers;
	for (std::size_t index = 0; index < RoutedLayerCount; ++index)
	{
		canceledLayers[index] = TakePointerState(m_layerStates[index]);
	}
	m_mouseCaptureLayer = InputLayer::System;
	m_bHasLastMousePosition = false;
	m_dispatch->EventDispatcher.CancelPointerEvents();

	if (canceledGlobal.IsMouseCaptured())
	{
		ReleaseWin32MouseCapture(m_windowHandle);
	}
	if (canceledGlobal.IsCursorHidden())
	{
		ShowWin32Cursor();
	}

	for (std::size_t index = 0; index < RoutedLayerCount; ++index)
	{
		ReleaseStateMouseButtons(canceledLayers[index], static_cast<InputLayer>(index + 1));
	}
	ReleaseStateMouseButtons(canceledGlobal, InputLayer::System);
}

void InputSystem::CancelLayer(InputLayer layer)
{
	InputState* state = FindLayerState(layer);
	if (state == nullptr)
	{
		return;
	}

	InputState canceled = std::move(*state);
	*state = InputState{};
	m_dispatch->EventDispatcher.CancelLayerEvents(layer);
	if (m_mouseCaptureLayer == layer)
	{
		ReleaseMouse();
		SetCursorVisibility(true);
	}

	ReleaseStateKeys(canceled, layer);
	ReleaseStateMouseButtons(canceled, layer);
}

void InputSystem::ReleaseStateKeys(const InputState& state, InputLayer layer)
{
	for (std::size_t index = 0; index < static_cast<std::size_t>(Key::Count); ++index)
	{
		const auto key = static_cast<Key>(index);
		if (state.IsKeyDown(key))
		{
			KeyboardEvent event{};
			event.KeyCode = key;
			event.Modifiers = state.GetModifiers();
			m_dispatch->EventDispatcher.DispatchCancellation(event, layer);
		}
	}
}

void InputSystem::ReleaseStateMouseButtons(const InputState& state, InputLayer layer)
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
			m_dispatch->EventDispatcher.DispatchCancellation(event, layer);
		}
	}
}
