#include "PCH.h"
#include "Input/InputSystemState.h"
#include "Window/Window.h"
#include "Window/SdlWindowInterop.h"

#include <utility>

bool InputSystem::SetRelativeMouseMode(bool bEnabled)
{
	if (!bEnabled)
	{
		return m_state->ReleaseRelativeMode();
	}
	if (m_state->MouseCaptureLayer)
	{
		return true;
	}
	Window* window = m_state->BoundWindow;
	if (window == nullptr || window->ShouldClose())
	{
		return false;
	}
	const auto epoch = m_state->CancellationEpoch;
	if (!SetSdlWindowRelativeMode(*window, true))
	{
		return false;
	}
	if (m_state->CancellationEpoch != epoch)
	{
		if (!SetSdlWindowRelativeMode(*window, false))
		{
			window->RequestClose();
		}
		return false;
	}
	m_state->MouseCaptureLayer = m_state->FocusRouter.GetActiveTargetLayer();
	return true;
}

bool InputSystem::State::ReleaseRelativeMode()
{
	const auto capturedLayer = std::exchange(MouseCaptureLayer, std::nullopt);
	return !capturedLayer || DisableNativeRelativeMode();
}

bool InputSystem::State::DisableNativeRelativeMode()
{
	if (BoundWindow == nullptr)
	{
		return true;
	}
	if (SetSdlWindowRelativeMode(*BoundWindow, false))
	{
		return true;
	}
	// Stop admitting camera work if native recovery failed; retain the owning SDL diagnostic.
	BoundWindow->RequestClose();
	return false;
}

bool InputSystem::IsMouseCaptured() const noexcept
{
	return m_state->MouseCaptureLayer.has_value();
}
