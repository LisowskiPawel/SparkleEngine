#include "PCH.h"
#include "Input/InputSystem.h"
#include "Input/InputDispatchState.h"
#include "Input/InputBackendResult.h"

const InputState& InputSystem::GetState(InputLayer layer) const noexcept
{
	m_ownerThread.AssertAccess();
	const auto index = static_cast<std::size_t>(layer);
	return index > 0 && index < LayerCount ? m_layerStates[index - 1] : m_inputState;
}

InputState* InputSystem::FindLayerState(InputLayer layer) noexcept
{
	const auto index = static_cast<std::size_t>(layer);
	return index > 0 && index < LayerCount ? &m_layerStates[index - 1] : nullptr;
}

void InputSystem::BeginInputRoutingFrame(bool bInteractionDisabled, bool bTextInputActive)
{
	m_ownerThread.AssertAccess();
	const InputLayer oldLayer = GetActiveLayer();
	m_dispatch->FocusRouter.BeginFrame(bInteractionDisabled, bTextInputActive);
	if (oldLayer != GetActiveLayer())
	{
		CancelLayer(oldLayer);
	}
}

void InputSystem::RegisterInputTargetRegion(float left, float top, float right, float bottom, InputLayer targetLayer)
{
	m_ownerThread.AssertAccess();
	m_dispatch->FocusRouter.RegisterTargetRegion(left, top, right, bottom, targetLayer);
}

void InputSystem::SetActiveLayer(InputLayer layer)
{
	m_ownerThread.AssertAccess();
	if (!IsLayerEnabled(layer))
	{
		layer = InputLayer::System;
	}

	const InputLayer oldLayer = GetActiveLayer();
	if (oldLayer == layer)
	{
		return;
	}

	m_dispatch->FocusRouter.SetActiveTargetLayer(layer);
	CancelLayer(oldLayer);
}

void InputSystem::SetLayerEnabled(InputLayer layer, bool bEnabled)
{
	m_ownerThread.AssertAccess();
	const auto index = static_cast<std::size_t>(layer);
	if (index == 0 || index >= LayerCount)
	{
		return;
	}
	if (m_layerEnabled[index] == bEnabled)
	{
		return;
	}

	if (!bEnabled)
	{
		if (GetActiveLayer() == layer)
		{
			m_dispatch->FocusRouter.SetActiveTargetLayer(InputLayer::System);
		}
		m_layerEnabled[index] = false;
		CancelLayer(layer);
	}
	else
	{
		m_layerEnabled[index] = true;
	}
}

bool InputSystem::IsLayerEnabled(InputLayer layer) const noexcept
{
	m_ownerThread.AssertAccess();
	const auto index = static_cast<std::size_t>(layer);
	return index < LayerCount && m_layerEnabled[index];
}

InputLayer InputSystem::GetActiveLayer() const noexcept
{
	m_ownerThread.AssertAccess();
	const InputLayer layer = m_dispatch->FocusRouter.GetActiveTargetLayer();
	return IsLayerEnabled(layer) ? layer : InputLayer::System;
}

InputLayer InputSystem::ResolveTargetLayer(const InputBackendResult& result)
{
	if (m_inputState.IsMouseCaptured() && IsLayerEnabled(m_mouseCaptureLayer))
	{
		return m_mouseCaptureLayer;
	}

	const InputLayer layer = m_dispatch->FocusRouter.ResolveTargetLayer(result);
	return IsLayerEnabled(layer) ? layer : InputLayer::System;
}
