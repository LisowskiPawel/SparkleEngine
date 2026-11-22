#pragma once

#include "Platform/Public/PlatformAPI.h"

#include "Events/EventHandle.h"
#include "Input/Dispatch/InputLayer.h"
#include "Input/Dispatch/DispatchMode.h"
#include "Input/Events/KeyboardEvent.h"
#include "Input/Events/MouseButtonEvent.h"
#include "Input/Events/MouseMoveEvent.h"
#include "Input/Events/MouseWheelEvent.h"
#include <functional>
#include <memory>

class Window;
class InputState;

template <typename TEvent> using InputCallback = std::function<void(const TEvent&)>;

using KeyboardCallback = InputCallback<KeyboardEvent>;
using MouseButtonCallback = InputCallback<MouseButtonEvent>;
using MouseMoveCallback = InputCallback<MouseMoveEvent>;
using MouseWheelCallback = InputCallback<MouseWheelEvent>;

// Operations and callbacks run on the creating thread. State is published before callbacks.
// Cancellation delivers releases synchronously to both delivery modes on each affected layer.
class SPARKLE_PLATFORM_API InputSystem final
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

	// Observes input without consuming messages. The Window outlives this binding.
	void SubscribeToWindow(Window& window);

	const InputState& GetState(InputLayer layer = InputLayer::System) const noexcept;

	void BeginInputRoutingFrame(bool bInteractionDisabled, bool bTextInputActive);
	void RegisterInputTargetRegion(float left, float top, float right, float bottom, InputLayer targetLayer);

	EventHandle SubscribeKeyboard(KeyboardCallback callback, InputLayer layer = InputLayer::Gameplay, DispatchMode mode = DispatchMode::Immediate);

	EventHandle SubscribeMouseButton(MouseButtonCallback callback, InputLayer layer = InputLayer::Gameplay, DispatchMode mode = DispatchMode::Immediate);

	EventHandle SubscribeMouseMove(MouseMoveCallback callback, InputLayer layer = InputLayer::Gameplay, DispatchMode mode = DispatchMode::Immediate);

	EventHandle SubscribeMouseWheel(MouseWheelCallback callback, InputLayer layer = InputLayer::Gameplay, DispatchMode mode = DispatchMode::Immediate);

	void Unsubscribe(EventHandle handle);

	// Successful relative mode owns camera routing and cursor hiding together.
	bool SetRelativeMouseMode(bool bEnabled);
	bool IsMouseCaptured() const noexcept;

private:
	struct State;
	std::unique_ptr<State> m_state;
};
