#include "PCH.h"
#include "Input/InputSystem.h"
#include "Input/Backends/Win32PointerControl.h"

void InputSystem::CaptureMouse()
{
	m_ownerThread.AssertAccess();
	if (CaptureWin32Mouse(m_windowHandle))
	{
		m_inputState.SetMouseCaptured(true);
		m_mouseCaptureLayer = GetActiveLayer();
	}
}

void InputSystem::ReleaseMouse()
{
	m_ownerThread.AssertAccess();
	const bool bWasCaptured = m_inputState.IsMouseCaptured();
	m_inputState.SetMouseCaptured(false);
	m_mouseCaptureLayer = InputLayer::System;
	if (bWasCaptured)
	{
		ReleaseWin32MouseCapture(m_windowHandle);
	}
}

bool InputSystem::IsMouseCaptured() const noexcept
{
	m_ownerThread.AssertAccess();
	return m_inputState.IsMouseCaptured();
}

void InputSystem::HideCursor()
{
	m_ownerThread.AssertAccess();
	if (!m_inputState.IsCursorHidden())
	{
		HideWin32Cursor();
		m_inputState.SetCursorHidden(true);
	}
}

void InputSystem::ShowCursor()
{
	m_ownerThread.AssertAccess();
	if (m_inputState.IsCursorHidden())
	{
		ShowWin32Cursor();
		m_inputState.SetCursorHidden(false);
	}
}

bool InputSystem::IsCursorHidden() const noexcept
{
	m_ownerThread.AssertAccess();
	return m_inputState.IsCursorHidden();
}

void InputSystem::CenterCursor(void* windowHandle)
{
	m_ownerThread.AssertAccess();
	if (const std::optional<MousePosition> center = CenterWin32Cursor(windowHandle))
	{
		m_lastMouseX = center->X;
		m_lastMouseY = center->Y;
		m_bHasLastMousePosition = true;
	}
}

void InputSystem::SetCursorVisibility(bool bVisible)
{
	if (bVisible)
	{
		ShowCursor();
	}
	else
	{
		HideCursor();
	}
}
