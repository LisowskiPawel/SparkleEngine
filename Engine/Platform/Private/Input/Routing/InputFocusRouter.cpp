#include "PCH.h"
#include "Input/Routing/InputFocusRouter.h"

InputFocusRouter::InputFocusRouter() noexcept = default;

InputFocusRouter::~InputFocusRouter() = default;

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

InputLayer InputFocusRouter::ResolveTargetLayer(const KeyboardEvent&) const noexcept
{
	if (m_bInteractionDisabled)
	{
		return InputLayer::UI;
	}
	if (m_bTextInputActive && m_activeTargetLayer != InputLayer::Gameplay)
	{
		return InputLayer::UI;
	}

	return m_activeTargetLayer;
}

InputLayer InputFocusRouter::ResolveTargetLayer(const MouseButtonEvent& event) const noexcept
{
	if (!m_bHasRoutingFrame)
	{
		return m_activeTargetLayer;
	}
	if (m_bInteractionDisabled)
	{
		return InputLayer::UI;
	}

	return event.bPressed ? ResolvePointTarget(event.Position) : m_activeTargetLayer;
}

InputLayer InputFocusRouter::ResolveTargetLayer(const MouseMoveEvent& event) const noexcept
{
	if (!m_bHasRoutingFrame)
	{
		return m_activeTargetLayer;
	}
	if (m_bInteractionDisabled || m_activeTargetLayer != InputLayer::Gameplay)
	{
		return InputLayer::UI;
	}

	return ResolvePointTarget(event.Position);
}

InputLayer InputFocusRouter::ResolveTargetLayer(const MouseWheelEvent& event) const noexcept
{
	if (!m_bHasRoutingFrame)
	{
		return m_activeTargetLayer;
	}

	return m_bInteractionDisabled ? InputLayer::UI : ResolvePointTarget(event.Position);
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
	const bool bNonEmptyBounds = Right > Left && Bottom > Top;
	return bRoutedLayer && bNonEmptyBounds;
}

bool InputFocusRouter::TargetRegion::Contains(MousePosition position) const noexcept
{
	const float x = static_cast<float>(position.X);
	const float y = static_cast<float>(position.Y);
	const bool bWithinX = x >= Left && x < Right;
	const bool bWithinY = y >= Top && y < Bottom;
	return bWithinX && bWithinY;
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
