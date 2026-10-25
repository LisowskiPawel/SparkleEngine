#include "PCH.h"
#include "Input/InputSystem.h"
#include "Input/Backends/Win32InputBackend.h"
#include "Input/Dispatch/InputEventDispatcher.h"
#include "Input/InputBackendResult.h"
#include "Input/Routing/InputFocusRouter.h"
#include "Window/Window.h"

#ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <memory>
#include <utility>

struct InputSystem::DispatchState final
{
	InputEventDispatcher EventDispatcher;
	InputFocusRouter FocusRouter;
};

InputSystem::InputSystem() :
    m_dispatch(std::make_unique<DispatchState>())
{
}

InputSystem::~InputSystem() = default;

const InputState& InputSystem::GetState(InputLayer layer) const noexcept
{
	if (layer == InputLayer::System)
	{
		return m_inputState;
	}

	const auto index = static_cast<std::size_t>(layer);
	return index < LayerCount ? m_layerStates[index] : m_inputState;
}

void InputSystem::BeginFrame()
{
	m_ownerThread.AssertAccess();
	m_inputState.BeginFrame();
	for (InputState& layerState : m_layerStates)
	{
		layerState.BeginFrame();
	}
	m_dispatch->EventDispatcher.ClearDeferredEvents();
}

void InputSystem::ProcessDeferredEvents()
{
	m_ownerThread.AssertAccess();

	m_dispatch->EventDispatcher.ProcessDeferredEvents(
	    m_layerEnabled,
	    [this]
	    {
		    if (m_captureQuery)
		    {
			    const bool wantsCaptureInput = m_captureQuery();
			    SetActiveLayer(wantsCaptureInput ? InputLayer::UI : InputLayer::Gameplay);
		    }
	    });
}

void InputSystem::SubscribeToWindow(Window& window)
{
	m_ownerThread.AssertAccess();
	auto handle = window.OnWindowMessage.Add([this](WindowMessageEvent& event) { HandleWindowMessage(event); });
	m_windowMessageHandle = ScopedEventHandle(window.OnWindowMessage, handle);
}

void InputSystem::HandleWindowMessage(WindowMessageEvent& event)
{
	if (OnWindowMessage(event.msg, event.wParam, event.lParam))
	{
		event.handled = true;
	}
}

bool InputSystem::OnWindowMessage(uint32_t message, uintptr_t parameter1, intptr_t parameter2)
{
	m_ownerThread.AssertAccess();
	InputBackendResult result = ProcessWin32InputMessage(message, parameter1, parameter2);

	if (!result.IsValid())
	{
		return false;
	}
	if (result.Type == InputEventType::MouseMove)
	{
		int32_t deltaX = 0;
		int32_t deltaY = 0;

		if (m_hasLastMousePosition)
		{
			deltaX = result.MouseMove.Position.X - m_lastMouseX;
			deltaY = result.MouseMove.Position.Y - m_lastMouseY;
		}

		m_lastMouseX = result.MouseMove.Position.X;
		m_lastMouseY = result.MouseMove.Position.Y;
		m_hasLastMousePosition = true;
		result.MouseMove.Delta.X = deltaX;
		result.MouseMove.Delta.Y = deltaY;
	}

	const InputLayer targetLayer = ResolveTargetLayer(result);
	if (result.Type == InputEventType::MouseButton && result.MouseButton.bPressed && targetLayer != InputLayer::System)
	{
		SetActiveLayer(targetLayer);
	}

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
			return false;
	}

	return true;
}

void InputSystem::BeginInputRoutingFrame(bool interactionDisabled, bool textInputActive)
{
	m_dispatch->FocusRouter.BeginFrame(interactionDisabled, textInputActive);
}

void InputSystem::RegisterInputTargetRegion(float left, float top, float right, float bottom, InputLayer targetLayer)
{
	m_dispatch->FocusRouter.RegisterTargetRegion(left, top, right, bottom, targetLayer);
}

void InputSystem::SetActiveLayer(InputLayer layer) noexcept
{
	if (!IsLayerEnabled(layer))
	{
		layer = InputLayer::System;
	}

	if (m_activeLayer == layer)
	{
		return;
	}

	const InputLayer oldLayer = m_activeLayer;
	m_activeLayer = layer;
	if (oldLayer != InputLayer::System)
	{
		CancelLayer(oldLayer);
	}
}

void InputSystem::SetLayerEnabled(InputLayer layer, bool enabled)
{
	const auto index = static_cast<std::size_t>(layer);
	if (index < LayerCount)
	{
		m_layerEnabled[index] = enabled;
		if (!enabled && m_activeLayer == layer)
		{
			SetActiveLayer(InputLayer::System);
		}
	}
}

void InputSystem::SetInputCaptureQuery(InputCaptureQuery query) noexcept
{
	m_captureQuery = std::move(query);
}

void InputSystem::ClearInputCaptureQuery() noexcept
{
	m_captureQuery = {};
}

bool InputSystem::IsLayerEnabled(InputLayer layer) const noexcept
{
	const auto index = static_cast<std::size_t>(layer);
	if (index < LayerCount)
	{
		return m_layerEnabled[index];
	}
	return false;
}

InputLayer InputSystem::GetActiveLayer() const noexcept
{
	return IsLayerEnabled(m_activeLayer) ? m_activeLayer : InputLayer::System;
}

InputLayer InputSystem::ResolveTargetLayer(const InputBackendResult& result)
{
	if (m_inputState.IsMouseCaptured() && IsLayerEnabled(m_mouseCaptureLayer))
	{
		return m_mouseCaptureLayer;
	}

	InputLayer targetLayer = m_dispatch->FocusRouter.ResolveTargetLayer(result, GetActiveLayer());

	return IsLayerEnabled(targetLayer) ? targetLayer : InputLayer::System;
}

EventHandle InputSystem::SubscribeKeyboard(KeyboardCallback callback, InputLayer layer, DispatchMode mode)
{
	m_ownerThread.AssertAccess();
	return m_dispatch->EventDispatcher.Subscribe<KeyboardEvent>(std::move(callback), layer, mode);
}

EventHandle InputSystem::SubscribeMouseButton(MouseButtonCallback callback, InputLayer layer, DispatchMode mode)
{
	m_ownerThread.AssertAccess();
	return m_dispatch->EventDispatcher.Subscribe<MouseButtonEvent>(std::move(callback), layer, mode);
}

EventHandle InputSystem::SubscribeMouseMove(MouseMoveCallback callback, InputLayer layer, DispatchMode mode)
{
	m_ownerThread.AssertAccess();
	return m_dispatch->EventDispatcher.Subscribe<MouseMoveEvent>(std::move(callback), layer, mode);
}

EventHandle InputSystem::SubscribeMouseWheel(MouseWheelCallback callback, InputLayer layer, DispatchMode mode)
{
	m_ownerThread.AssertAccess();
	return m_dispatch->EventDispatcher.Subscribe<MouseWheelEvent>(std::move(callback), layer, mode);
}

void InputSystem::Unsubscribe(EventHandle handle)
{
	m_ownerThread.AssertAccess();
	m_dispatch->EventDispatcher.Unsubscribe(handle);
}

void InputSystem::CaptureMouse()
{
	HWND hWnd = GetForegroundWindow();
	if (hWnd)
	{
		SetCapture(hWnd);
		m_inputState.SetMouseCaptured(true);
		m_mouseCaptureLayer = GetActiveLayer();
	}
}

void InputSystem::ReleaseMouse()
{
	ReleaseCapture();
	m_inputState.SetMouseCaptured(false);
	m_mouseCaptureLayer = InputLayer::System;
}

bool InputSystem::IsMouseCaptured() const noexcept
{
	return m_inputState.IsMouseCaptured();
}

void InputSystem::HideCursor()
{
	while (::ShowCursor(FALSE) >= 0)
	{
		continue;
	}
	m_inputState.SetCursorHidden(true);
}

void InputSystem::ShowCursor()
{
	while (::ShowCursor(TRUE) < 0)
	{
		continue;
	}
	m_inputState.SetCursorHidden(false);
}

bool InputSystem::IsCursorHidden() const noexcept
{
	return m_inputState.IsCursorHidden();
}

void InputSystem::CenterCursor(void* windowHandle)
{
	HWND hWnd = static_cast<HWND>(windowHandle);
	if (!hWnd)
	{
		return;
	}

	RECT rect;
	if (GetClientRect(hWnd, &rect))
	{
		POINT center;
		center.x = (rect.right - rect.left) / 2;
		center.y = (rect.bottom - rect.top) / 2;
		ClientToScreen(hWnd, &center);
		SetCursorPos(center.x, center.y);

		ScreenToClient(hWnd, &center);
		m_lastMouseX = center.x;
		m_lastMouseY = center.y;
	}
}

void InputSystem::SetCursorVisibility(bool visible)
{
	const bool currentlyHidden = m_inputState.IsCursorHidden();
	if (visible && !currentlyHidden)
	{
		return;
	}
	if (!visible && currentlyHidden)
	{
		return;
	}

	if (visible)
	{
		ShowCursor();
	}
	else
	{
		HideCursor();
	}
}

void InputSystem::CancelLayer(InputLayer layer)
{
	if (layer == InputLayer::System)
	{
		return;
	}

	const auto layerIndex = static_cast<std::size_t>(layer);
	if (layerIndex >= LayerCount)
	{
		return;
	}

	InputState& state = m_layerStates[layerIndex];
	for (std::size_t keyIndex = 0; keyIndex < InputState::KeyCount; ++keyIndex)
	{
		const ButtonState keyState = state.m_KeyStates[keyIndex];
		if (keyState != ButtonState::Pressed && keyState != ButtonState::Held)
		{
			continue;
		}

		KeyboardEvent releaseEvent{};
		releaseEvent.KeyCode = static_cast<Key>(keyIndex);
		releaseEvent.Modifiers = state.GetModifiers();
		releaseEvent.bPressed = false;
		state.SetKeyState(releaseEvent.KeyCode, ButtonState::Released);
		m_dispatch->EventDispatcher.DispatchImmediate(releaseEvent, layer, m_layerEnabled);
	}

	const MousePosition mousePosition = state.GetMousePosition();
	for (std::size_t buttonIndex = 0; buttonIndex < InputState::MouseButtonCount; ++buttonIndex)
	{
		const ButtonState buttonState = state.m_MouseButtonStates[buttonIndex];
		if (buttonState != ButtonState::Pressed && buttonState != ButtonState::Held)
		{
			continue;
		}

		MouseButtonEvent releaseEvent{};
		releaseEvent.Button = static_cast<MouseButton>(buttonIndex);
		releaseEvent.Position = mousePosition;
		releaseEvent.Modifiers = state.GetModifiers();
		releaseEvent.bPressed = false;
		state.SetMouseButtonState(releaseEvent.Button, ButtonState::Released);
		m_dispatch->EventDispatcher.DispatchImmediate(releaseEvent, layer, m_layerEnabled);
	}

	state = InputState{};
}

template <typename TEvent> void InputSystem::ProcessEvent(const TEvent& event, InputLayer TargetLayer)
{
	UpdateStateFromEvent(m_inputState, event);
	if (TargetLayer == InputLayer::System || IsLayerEnabled(TargetLayer))
	{
		UpdateStateFromEvent(m_layerStates[static_cast<std::size_t>(TargetLayer)], event);
	}
	m_dispatch->EventDispatcher.DispatchImmediate(event, TargetLayer, m_layerEnabled);
	m_dispatch->EventDispatcher.QueueDeferred(event, TargetLayer, m_layerEnabled);
}

void InputSystem::UpdateStateFromEvent(InputState& state, const KeyboardEvent& event)
{
	state.SetKeyState(event.KeyCode, event.bPressed ? ButtonState::Pressed : ButtonState::Released);
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
