#include "PCH.h"
#include "Input/InputSystem.h"
#include "Input/InputDispatchState.h"
#include "Input/InputBackendResult.h"
#include "Input/Backends/Win32InputBackend.h"
#include "Window/Window.h"

#include <memory>
#include <utility>

InputSystem::InputSystem() :
    m_dispatch(std::make_unique<DispatchState>())
{
}

InputSystem::~InputSystem()
{
	m_windowMessageHandle.Reset();
	ReleaseMouse();
	SetCursorVisibility(true);
}

void InputSystem::BeginFrame()
{
	m_ownerThread.AssertAccess();
	AdvanceFrameState();
	m_dispatch->EventDispatcher.ClearDeferredEvents();
}

void InputSystem::ProcessDeferredEvents()
{
	m_ownerThread.AssertAccess();
	m_dispatch->EventDispatcher.ProcessDeferredEvents(m_layerEnabled);
}

void InputSystem::SubscribeToWindow(Window& window)
{
	m_ownerThread.AssertAccess();
	CancelInput();
	m_windowMessageHandle.Reset();
	m_windowHandle = window.GetHWND();

	auto handle = window.OnWindowMessage.Add([this](WindowMessageEvent& event) { HandleWindowMessage(event); });
	m_windowMessageHandle = ScopedEventHandle(window.OnWindowMessage, handle);
}

void InputSystem::HandleWindowMessage(const WindowMessageEvent& event)
{
	m_ownerThread.AssertAccess();
	InputBackendResult result = ProcessWin32InputMessage(event);
	if (!result.IsValid())
	{
		return;
	}

	if (result.Type == InputEventType::FocusLost)
	{
		CancelInput();
		return;
	}
	if (result.Type == InputEventType::CaptureLost)
	{
		CancelPointerInput();
		return;
	}
	if (result.Type == InputEventType::MouseMove)
	{
		UpdateMouseMoveDelta(result.MouseMove);
	}

	const InputLayer targetLayer = ResolveTargetLayer(result);
	const bool bMousePress = result.Type == InputEventType::MouseButton && result.MouseButton.bPressed;
	if (bMousePress && targetLayer != InputLayer::System)
	{
		SetActiveLayer(targetLayer);
	}

	DispatchInputEvent(result, targetLayer);
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
