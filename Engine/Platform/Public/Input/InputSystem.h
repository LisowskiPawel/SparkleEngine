#pragma once

#include "Platform/Public/PlatformAPI.h"

#include "Events/EventHandle.h"
#include "Events/ScopedEventHandle.h"
#include "Input/InputState.h"
#include "Input/Dispatch/InputLayer.h"
#include "Input/Dispatch/DispatchMode.h"
#include "Input/Events/KeyboardEvent.h"
#include "Input/Events/MouseButtonEvent.h"
#include "Input/Events/MouseMoveEvent.h"
#include "Input/Events/MouseWheelEvent.h"
#include "Core/Public/Threading/ThreadOwnership.h"

#include <cstddef>
#include <array>
#include <cstdint>
#include <functional>
#include <memory>

class Window;
struct WindowMessageEvent;
struct InputBackendResult;

template <typename TEvent> using InputCallback = std::function<void(const TEvent&)>;

using KeyboardCallback = InputCallback<KeyboardEvent>;
using MouseButtonCallback = InputCallback<MouseButtonEvent>;
using MouseMoveCallback = InputCallback<MouseMoveEvent>;
using MouseWheelCallback = InputCallback<MouseWheelEvent>;

// Operations and callbacks run on the creating thread. State is published before callbacks.
// Cancellation delivers releases synchronously to both delivery modes on each affected layer.
class SPARKLE_PLATFORM_API InputSystem
{
public:
	InputSystem();

	~InputSystem();

	InputSystem(const InputSystem&) = delete;
	InputSystem& operator=(const InputSystem&) = delete;

	InputSystem(InputSystem&&) = delete;
	InputSystem& operator=(InputSystem&&) = delete;

	void BeginFrame();

	void ProcessDeferredEvents();

	void SubscribeToWindow(Window& window);

	// Observes input without consuming the native message. The subscribed Window outlives this binding.
	void HandleWindowMessage(const WindowMessageEvent& event);

	const InputState& GetState() const noexcept { return GetState(InputLayer::System); }

	const InputState& GetState(InputLayer layer) const noexcept;

	void BeginInputRoutingFrame(bool bInteractionDisabled, bool bTextInputActive);
	void RegisterInputTargetRegion(float left, float top, float right, float bottom, InputLayer targetLayer);

	void SetActiveLayer(InputLayer layer);
	void SetLayerEnabled(InputLayer layer, bool bEnabled);

	bool IsLayerEnabled(InputLayer layer) const noexcept;

	InputLayer GetActiveLayer() const noexcept;

	EventHandle SubscribeKeyboard(KeyboardCallback callback, InputLayer layer = InputLayer::Gameplay, DispatchMode mode = DispatchMode::Immediate);

	EventHandle SubscribeMouseButton(MouseButtonCallback callback, InputLayer layer = InputLayer::Gameplay, DispatchMode mode = DispatchMode::Immediate);

	EventHandle SubscribeMouseMove(MouseMoveCallback callback, InputLayer layer = InputLayer::Gameplay, DispatchMode mode = DispatchMode::Immediate);

	EventHandle SubscribeMouseWheel(MouseWheelCallback callback, InputLayer layer = InputLayer::Gameplay, DispatchMode mode = DispatchMode::Immediate);

	void Unsubscribe(EventHandle handle);

	void CaptureMouse();

	void ReleaseMouse();

	bool IsMouseCaptured() const noexcept;

	void HideCursor();

	void ShowCursor();

	bool IsCursorHidden() const noexcept;

	void SetCursorVisibility(bool bVisible);

	void CenterCursor(void* windowHandle);

private:
	struct DispatchState;

	static constexpr std::size_t LayerCount = static_cast<std::size_t>(InputLayer::Count);
	static constexpr std::size_t RoutedLayerCount = LayerCount - 1;

	InputLayer ResolveTargetLayer(const InputBackendResult& result);
	InputState* FindLayerState(InputLayer layer) noexcept;
	void AdvanceFrameState();
	void UpdateMouseMoveDelta(MouseMoveEvent& event) noexcept;
	void DispatchInputEvent(const InputBackendResult& result, InputLayer targetLayer);
	void CancelInput();
	void CancelPointerInput();
	void CancelLayer(InputLayer layer);
	static InputState TakePointerState(InputState& state) noexcept;
	void ReleaseStateKeys(const InputState& state, InputLayer layer);
	void ReleaseStateMouseButtons(const InputState& state, InputLayer layer);

	template <typename TEvent> void ProcessEvent(const TEvent& event, InputLayer targetLayer);

	void UpdateStateFromEvent(InputState& state, const KeyboardEvent& event);
	void UpdateStateFromEvent(InputState& state, const MouseButtonEvent& event);
	void UpdateStateFromEvent(InputState& state, const MouseMoveEvent& event);
	void UpdateStateFromEvent(InputState& state, const MouseWheelEvent& event);

	std::unique_ptr<DispatchState> m_dispatch;

	InputState m_inputState;
	std::array<InputState, RoutedLayerCount> m_layerStates{};

	Threading::OwnerThread m_ownerThread{"InputSystem registration and dispatch"};

	std::array<bool, LayerCount> m_layerEnabled = {true, true, true};
	InputLayer m_mouseCaptureLayer = InputLayer::System;

	int32_t m_lastMouseX = 0;
	int32_t m_lastMouseY = 0;
	bool m_bHasLastMousePosition = false;
	void* m_windowHandle = nullptr;

	ScopedEventHandle m_windowMessageHandle;
};
