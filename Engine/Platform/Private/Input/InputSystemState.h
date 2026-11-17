#pragma once

#include "Input/InputSystem.h"
#include "Input/InputState.h"
#include "Input/Dispatch/InputEventDispatcher.h"
#include "Input/Routing/InputFocusRouter.h"
#include "Events/ScopedEventHandle.h"
#include "Threading/ThreadOwnership.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <variant>

struct WindowMessageEvent;
struct InputFocusLost;
struct InputCaptureLost;

struct InputSystem::State final
{
	static constexpr std::size_t RoutedLayerCount = static_cast<std::size_t>(InputLayer::Count) - 1;

	void HandleWindowMessage(const WindowMessageEvent& event);
	InputState* FindLayerState(InputLayer layer) noexcept;
	void SetActiveLayer(InputLayer layer);
	void UpdateMouseMoveDelta(MouseMoveEvent& event) noexcept;

	void ReleaseMouse();
	void SetCursorVisibility(bool bVisible);

	void CancelInput();
	void CancelPointerInput();
	void CancelLayer(InputLayer layer);
	static InputState TakePointerState(InputState& state) noexcept;

	void ReleaseStateKeys(const InputState& state, InputLayer layer);
	void ReleaseStateMouseButtons(const InputState& state, InputLayer layer);

	template <typename TEvent> void ProcessEvent(const TEvent& event);
	void ProcessEvent(const std::monostate&) noexcept;
	void ProcessEvent(const InputFocusLost&);
	void ProcessEvent(const InputCaptureLost&);

	static void UpdateStateFromEvent(InputState& state, const KeyboardEvent& event);
	static void UpdateStateFromEvent(InputState& state, const MouseButtonEvent& event);
	static void UpdateStateFromEvent(InputState& state, const MouseMoveEvent& event);
	static void UpdateStateFromEvent(InputState& state, const MouseWheelEvent& event);

	InputEventDispatcher Dispatcher;
	InputFocusRouter FocusRouter;
	InputState GlobalState;
	std::array<InputState, RoutedLayerCount> LayerStates;
	Threading::OwnerThread OwnerThread{"InputSystem registration and dispatch"};
	InputLayer MouseCaptureLayer = InputLayer::System;
	std::uint64_t CancellationEpoch = 0;
	std::int32_t LastMouseX = 0;
	std::int32_t LastMouseY = 0;
	bool bHasLastMousePosition = false;
	void* WindowHandle = nullptr;
	ScopedEventHandle WindowMessageHandle;
};
