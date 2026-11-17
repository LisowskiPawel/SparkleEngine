#include "PCH.h"
#include "Input/InputSystemState.h"
#include "Input/Backends/Win32PointerControl.h"

void InputSystem::CaptureMouse()
{
	m_state->OwnerThread.AssertAccess();
	if (CaptureWin32Mouse(m_state->WindowHandle))
	{
		m_state->GlobalState.SetMouseCaptured(true);
		m_state->MouseCaptureLayer = m_state->FocusRouter.GetActiveTargetLayer();
	}
}

void InputSystem::ReleaseMouse()
{
	m_state->OwnerThread.AssertAccess();
	m_state->ReleaseMouse();
}

void InputSystem::State::ReleaseMouse()
{
	const bool bWasCaptured = GlobalState.IsMouseCaptured();
	GlobalState.SetMouseCaptured(false);
	MouseCaptureLayer = InputLayer::System;
	if (bWasCaptured)
	{
		ReleaseWin32MouseCapture(WindowHandle);
	}
}

bool InputSystem::IsMouseCaptured() const noexcept
{
	m_state->OwnerThread.AssertAccess();
	return m_state->GlobalState.IsMouseCaptured();
}

void InputSystem::CenterCursor()
{
	m_state->OwnerThread.AssertAccess();
	if (const std::optional<MousePosition> center = CenterWin32Cursor(m_state->WindowHandle))
	{
		m_state->LastMouseX = center->X;
		m_state->LastMouseY = center->Y;
		m_state->bHasLastMousePosition = true;
	}
}

void InputSystem::SetCursorVisibility(bool bVisible)
{
	m_state->OwnerThread.AssertAccess();
	m_state->SetCursorVisibility(bVisible);
}

void InputSystem::State::SetCursorVisibility(bool bVisible)
{
	if (GlobalState.IsCursorHidden() == !bVisible)
	{
		return;
	}

	if (bVisible)
	{
		ShowWin32Cursor();
	}
	else
	{
		HideWin32Cursor();
	}
	GlobalState.SetCursorHidden(!bVisible);
}
