#include "PCH.h"
#include "Input/Routing/InputFocusRouter.h"
#include "Input/InputBackendResult.h"

void InputFocusRouter::BeginFrame(bool bInteractionDisabled, bool bTextInputActive)
{
	m_targetRegions.clear();
	m_bInteractionDisabled = bInteractionDisabled;
	m_bTextInputActive = bTextInputActive;
	const bool bUiFocusRequested = m_bInteractionDisabled || m_bTextInputActive;
	if (!m_bHasRoutingFrame || bUiFocusRequested)
	{
		m_activeTargetLayer = InputLayer::UI;
	}
	m_bHasRoutingFrame = true;
}

void InputFocusRouter::RegisterTargetRegion(float left, float top, float right, float bottom, InputLayer targetLayer)
{
	TargetRegion region{};
	region.Left = left;
	region.Top = top;
	region.Right = right;
	region.Bottom = bottom;
	region.TargetLayer = targetLayer;
	if (region.IsValid())
	{
		m_targetRegions.push_back(region);
	}
}

InputLayer InputFocusRouter::ResolveTargetLayer(const InputBackendResult& result) const noexcept
{
	if (!m_bHasRoutingFrame)
	{
		return m_activeTargetLayer;
	}

	if (m_bInteractionDisabled)
	{
		return InputLayer::UI;
	}

	switch (result.Type)
	{
		case InputEventType::MouseButton:
			if (result.MouseButton.bPressed)
			{
				return ResolvePointTarget(result.MouseButton.Position);
			}
			break;
		case InputEventType::MouseMove:
			return ResolveMouseMoveTarget(result.MouseMove.Position);
		case InputEventType::MouseWheel:
			return ResolvePointTarget(result.MouseWheel.Position);
		case InputEventType::Keyboard:
			if (m_bTextInputActive && m_activeTargetLayer != InputLayer::Gameplay)
			{
				return InputLayer::UI;
			}
			return GetActiveTargetLayer();
		default:
			break;
	}

	return GetActiveTargetLayer();
}

InputLayer InputFocusRouter::GetActiveTargetLayer() const noexcept
{
	return m_activeTargetLayer;
}

void InputFocusRouter::SetActiveTargetLayer(InputLayer layer) noexcept
{
	m_activeTargetLayer = layer;
	if (layer == InputLayer::Gameplay)
	{
		m_bTextInputActive = false;
	}
}

void InputFocusRouter::ResetFocus() noexcept
{
	m_activeTargetLayer = m_bHasRoutingFrame ? InputLayer::UI : InputLayer::Gameplay;
	m_bTextInputActive = false;
}

bool InputFocusRouter::TargetRegion::IsValid() const noexcept
{
	const bool bRoutedLayer = TargetLayer == InputLayer::Gameplay || TargetLayer == InputLayer::UI;
	return bRoutedLayer && Right > Left && Bottom > Top;
}

bool InputFocusRouter::TargetRegion::Contains(MousePosition position) const noexcept
{
	const float x = static_cast<float>(position.X);
	const float y = static_cast<float>(position.Y);
	return x >= Left && x < Right && y >= Top && y < Bottom;
}

InputLayer InputFocusRouter::ResolvePointTarget(MousePosition position) const noexcept
{
	for (auto region = m_targetRegions.rbegin(); region != m_targetRegions.rend(); ++region)
	{
		if (region->Contains(position))
		{
			return region->TargetLayer;
		}
	}

	return InputLayer::UI;
}

InputLayer InputFocusRouter::ResolveMouseMoveTarget(MousePosition position) const noexcept
{
	if (m_activeTargetLayer != InputLayer::Gameplay)
	{
		return InputLayer::UI;
	}

	return ResolvePointTarget(position) == InputLayer::Gameplay ? InputLayer::Gameplay : InputLayer::UI;
}
