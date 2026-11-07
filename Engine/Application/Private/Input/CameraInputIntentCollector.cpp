#include "PCH.h"

#include "Input/CameraInputIntentCollector.h"

#include "Input/Dispatch/InputLayer.h"
#include "Input/Events/MouseMoveEvent.h"
#include "Input/Events/MouseWheelEvent.h"
#include "Input/InputState.h"
#include "Input/InputSystem.h"
#include "Input/Keyboard/Key.h"
#include "Input/Mouse/MouseButton.h"
#include "Window/Window.h"

CameraInputIntentCollector::CameraInputIntentCollector(InputSystem& inputSystem, Window& window) noexcept :
    m_inputSystem(inputSystem),
    m_window(window)
{
	m_mouseButtonHandle = m_inputSystem.SubscribeMouseButton([this](const MouseButtonEvent& event) { OnMouseButton(event); }, InputLayer::Gameplay);
	m_mouseMoveHandle = m_inputSystem.SubscribeMouseMove([this](const MouseMoveEvent& event) { OnMouseMove(event); }, InputLayer::Gameplay);
	m_keyboardHandle = m_inputSystem.SubscribeKeyboard([this](const KeyboardEvent& event) { OnKeyboard(event); }, InputLayer::Gameplay);
	m_mouseWheelHandle = m_inputSystem.SubscribeMouseWheel([this](const MouseWheelEvent& event) { OnMouseWheel(event); }, InputLayer::Gameplay);
}

CameraInputIntentCollector::~CameraInputIntentCollector() noexcept
{
	m_inputSystem.Unsubscribe(m_mouseButtonHandle);
	m_inputSystem.Unsubscribe(m_mouseMoveHandle);
	m_inputSystem.Unsubscribe(m_keyboardHandle);
	m_inputSystem.Unsubscribe(m_mouseWheelHandle);
	EndMouseLook();
}

CameraInputIntent CameraInputIntentCollector::Collect(float aspectRatio) noexcept
{
	const bool bCameraInputBlocked = GetForegroundWindow() != m_window.GetHWND() || m_inputSystem.GetState(InputLayer::Gameplay).IsMouseButtonDown(MouseButton::Right) == false;

	if (bCameraInputBlocked)
	{
		EndMouseLook();
	}
	const InputState& inputState = m_inputSystem.GetState(InputLayer::Gameplay);
	const bool bForwardHeld = inputState.IsKeyDown(Key::W) || inputState.IsKeyDown(Key::Up);
	const bool bBackwardHeld = inputState.IsKeyDown(Key::S) || inputState.IsKeyDown(Key::Down);
	const bool bRightHeld = inputState.IsKeyDown(Key::D) || inputState.IsKeyDown(Key::Right);
	const bool bLeftHeld = inputState.IsKeyDown(Key::A) || inputState.IsKeyDown(Key::Left);
	const bool bUpHeld = inputState.IsKeyDown(Key::E) || inputState.IsKeyDown(Key::Space);
	const bool bDownHeld = inputState.IsKeyDown(Key::Q) || inputState.IsKeyDown(Key::C);

	CameraInputIntent intent;
	intent.ForwardAxis = (bForwardHeld ? 1.0f : 0.0f) - (bBackwardHeld ? 1.0f : 0.0f);
	intent.RightAxis = (bRightHeld ? 1.0f : 0.0f) - (bLeftHeld ? 1.0f : 0.0f);
	intent.UpAxis = (bUpHeld ? 1.0f : 0.0f) - (bDownHeld ? 1.0f : 0.0f);
	intent.LookDeltaX = m_lookDeltaX;
	intent.LookDeltaY = m_lookDeltaY;
	intent.SpeedStepCount = m_speedStepCount;
	intent.bSprint = inputState.IsKeyDown(Key::LeftShift) || inputState.IsKeyDown(Key::RightShift);
	intent.bHasAspectRatio = aspectRatio > 0.0f;
	intent.AspectRatio = intent.bHasAspectRatio ? aspectRatio : 1.0f;
	m_lookDeltaX = 0.0f;
	m_lookDeltaY = 0.0f;
	m_speedStepCount = 0.0f;
	return intent;
}

void CameraInputIntentCollector::OnMouseButton(const MouseButtonEvent& event) noexcept
{
	if (event.Button != MouseButton::Right)
	{
		return;
	}
	if (event.IsPressed() && !m_bMouseLookActive)
	{
		m_bMouseLookActive = true;
		m_inputSystem.CaptureMouse();
		m_inputSystem.SetCursorVisibility(false);
	}
	else if (event.IsReleased() && m_bMouseLookActive)
	{
		EndMouseLook();
	}
}

void CameraInputIntentCollector::OnKeyboard(const KeyboardEvent& event) noexcept
{
	if (event.KeyCode == Key::Escape && event.IsPressed())
	{
		EndMouseLook();
	}
}

void CameraInputIntentCollector::OnMouseMove(const MouseMoveEvent& event) noexcept
{
	if (!m_bMouseLookActive)
	{
		return;
	}
	m_lookDeltaX += static_cast<float>(event.Delta.X);
	m_lookDeltaY += static_cast<float>(event.Delta.Y);
	m_inputSystem.CenterCursor(m_window.GetHWND());
}

void CameraInputIntentCollector::OnMouseWheel(const MouseWheelEvent& event) noexcept
{
	if (event.IsVertical())
	{
		m_speedStepCount += event.Delta;
	}
}

void CameraInputIntentCollector::EndMouseLook() noexcept
{
	if (!m_bMouseLookActive)
	{
		return;
	}

	m_bMouseLookActive = false;
	m_lookDeltaX = 0.0f;
	m_lookDeltaY = 0.0f;
	m_inputSystem.ReleaseMouse();
	m_inputSystem.SetCursorVisibility(true);
}
