#include "PCH.h"
#include "Input/InputSystemState.h"

const InputState& InputSystem::GetState(InputLayer layer) const noexcept
{
	const auto index = static_cast<std::size_t>(layer);
	return index > 0 && index <= State::RoutedLayerCount ? m_state->GetRoutedState(layer) : m_state->GlobalState;
}

InputState& InputSystem::State::GetRoutedState(InputLayer layer) noexcept
{
	return LayerStates[static_cast<std::size_t>(layer) - 1];
}

void InputSystem::BeginInputRoutingFrame(bool bInteractionDisabled, bool bTextInputActive)
{
	const InputLayer oldLayer = m_state->FocusRouter.GetActiveTargetLayer();
	m_state->FocusRouter.BeginFrame(bInteractionDisabled, bTextInputActive);
	if (oldLayer != m_state->FocusRouter.GetActiveTargetLayer())
	{
		m_state->CancelLayer(oldLayer);
	}
}

void InputSystem::RegisterInputTargetRegion(float left, float top, float right, float bottom, InputLayer targetLayer)
{
	m_state->FocusRouter.RegisterTargetRegion(left, top, right, bottom, targetLayer);
}

void InputSystem::State::SetActiveLayer(InputLayer layer)
{
	const InputLayer oldLayer = FocusRouter.GetActiveTargetLayer();
	if (oldLayer == layer)
	{
		return;
	}

	FocusRouter.SetActiveTargetLayer(layer);
	CancelLayer(oldLayer);
}
