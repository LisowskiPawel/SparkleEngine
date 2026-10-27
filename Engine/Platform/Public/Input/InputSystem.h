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
using InputCaptureQuery = std::function<bool()>;

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

	void HandleWindowMessage(WindowMessageEvent& event);

	bool OnWindowMessage(uint32_t message, uintptr_t parameter1, intptr_t parameter2);

	const InputState& GetState() const noexcept { return m_inputState; }

	const InputState& GetState(InputLayer layer) const noexcept;

	void BeginInputRoutingFrame(bool interactionDisabled, bool textInputActive);
	void RegisterInputTargetRegion(float left, float top, float right, float bottom, InputLayer targetLayer);

	void SetActiveLayer(InputLayer layer) noexcept;
	void SetLayerEnabled(InputLayer layer, bool enabled);
	void SetInputCaptureQuery(InputCaptureQuery query) noexcept;
	void ClearInputCaptureQuery() noexcept;

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

	void SetCursorVisibility(bool visible);

	void CenterCursor(void* windowHandle);

private:
	struct DispatchState;

	static constexpr std::size_t LayerCount = static_cast<std::size_t>(InputLayer::Count);

	InputLayer ResolveTargetLayer(const InputBackendResult& result);
	void UpdateMouseMoveDelta(MouseMoveEvent& event) noexcept;
	bool DispatchInputEvent(const InputBackendResult& result, InputLayer targetLayer);
	void CancelLayer(InputLayer layer);
	void ReleaseLayerKeys(InputState& state, InputLayer layer);
	void ReleaseLayerMouseButtons(InputState& state, InputLayer layer);

	template <typename TEvent> void ProcessEvent(const TEvent& event, InputLayer TargetLayer);

	void UpdateStateFromEvent(InputState& state, const KeyboardEvent& event);
	void UpdateStateFromEvent(InputState& state, const MouseButtonEvent& event);
	void UpdateStateFromEvent(InputState& state, const MouseMoveEvent& event);
	void UpdateStateFromEvent(InputState& state, const MouseWheelEvent& event);

	std::unique_ptr<DispatchState> m_dispatch;

	InputState m_inputState;
	std::array<InputState, LayerCount> m_layerStates{};

	Threading::OwnerThread m_ownerThread{"InputSystem registration and dispatch"};

	std::array<bool, LayerCount> m_layerEnabled = {true, true, true};
	InputLayer m_activeLayer = InputLayer::Gameplay;
	InputLayer m_mouseCaptureLayer = InputLayer::System;
	InputCaptureQuery m_captureQuery;

	int32_t m_lastMouseX = 0;
	int32_t m_lastMouseY = 0;
	bool m_hasLastMousePosition = false;

	ScopedEventHandle m_windowMessageHandle;
};
