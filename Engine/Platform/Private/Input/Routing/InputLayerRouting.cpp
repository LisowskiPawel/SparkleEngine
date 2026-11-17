#include "PCH.h"
#include "Input/InputSystemState.h"

const InputState& InputSystem::GetState(InputLayer layer) const noexcept
{
	m_state->OwnerThread.AssertAccess();
	const InputState* state = m_state->FindLayerState(layer);
	return state != nullptr ? *state : m_state->GlobalState;
}

InputState* InputSystem::State::FindLayerState(InputLayer layer) noexcept
{
	const auto index = static_cast<std::size_t>(layer);
	return index > 0 && index <= RoutedLayerCount ? &LayerStates[index - 1] : nullptr;
}

void InputSystem::BeginInputRoutingFrame(bool bInteractionDisabled, bool bTextInputActive)
{
	m_state->OwnerThread.AssertAccess();
	const InputLayer oldLayer = m_state->FocusRouter.GetActiveTargetLayer();
	m_state->FocusRouter.BeginFrame(bInteractionDisabled, bTextInputActive);
	if (oldLayer != m_state->FocusRouter.GetActiveTargetLayer())
	{
		m_state->CancelLayer(oldLayer);
	}
}

void InputSystem::RegisterInputTargetRegion(float left, float top, float right, float bottom, InputLayer targetLayer)
{
	m_state->OwnerThread.AssertAccess();
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
