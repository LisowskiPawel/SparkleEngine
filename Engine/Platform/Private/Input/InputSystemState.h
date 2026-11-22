#pragma once

#include "Input/InputSystem.h"
#include "Input/InputState.h"
#include "Input/Dispatch/InputEventDispatcher.h"
#include "Input/Routing/InputFocusRouter.h"
#include "Events/ScopedEventHandle.h"

#include <array>
#include <cstddef>
#include <cstdint>

struct SdlWindowEvent;

struct InputSystem::State final
{
	static constexpr std::size_t RoutedLayerCount = static_cast<std::size_t>(InputLayer::Count) - 1;

	void HandleWindowEvent(const SdlWindowEvent& event);
	InputState& GetRoutedState(InputLayer layer) noexcept;
	void SetActiveLayer(InputLayer layer);

	bool ReleaseRelativeMode();
	bool DisableNativeRelativeMode();

	void CancelInput();
	void CancelPointerInput();
	void CancelLayer(InputLayer layer);
	static InputState TakePointerState(InputState& state) noexcept;

	void ReleaseStateKeys(const InputState& state, InputLayer layer);
	void ReleaseStateMouseButtons(const InputState& state, InputLayer layer);

	template <typename TEvent> void ProcessEvent(const TEvent& event);

	static void UpdateStateFromEvent(InputState& state, const KeyboardEvent& event);
	static void UpdateStateFromEvent(InputState& state, const MouseButtonEvent& event);
	static void UpdateStateFromEvent(InputState& state, const MouseMoveEvent& event);
	static void UpdateStateFromEvent(InputState& state, const MouseWheelEvent& event);

	InputEventDispatcher Dispatcher;
	InputFocusRouter FocusRouter;
	InputState GlobalState;
	std::array<InputState, RoutedLayerCount> LayerStates;
	InputLayer MouseCaptureLayer = InputLayer::System;
	std::uint64_t CancellationEpoch = 0;
	Window* BoundWindow = nullptr;
	ScopedEventHandle WindowEventHandle;
	ScopedEventHandle WindowCancellationHandle;
};
