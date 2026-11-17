#pragma once

#include "Input/Dispatch/InputLayer.h"
#include "Input/Mouse/MousePosition.h"
#include "Input/Events/KeyboardEvent.h"
#include "Input/Events/MouseButtonEvent.h"
#include "Input/Events/MouseMoveEvent.h"
#include "Input/Events/MouseWheelEvent.h"

#include <vector>

class InputFocusRouter final
{
public:
	InputFocusRouter() noexcept;

	~InputFocusRouter();

	InputFocusRouter(const InputFocusRouter&) = delete;
	InputFocusRouter& operator=(const InputFocusRouter&) = delete;
	InputFocusRouter(InputFocusRouter&&) = delete;
	InputFocusRouter& operator=(InputFocusRouter&&) = delete;

	void BeginFrame(bool bInteractionDisabled, bool bTextInputActive);
	void RegisterTargetRegion(float left, float top, float right, float bottom, InputLayer targetLayer);

	InputLayer ResolveTargetLayer(const KeyboardEvent& event) const noexcept;
	InputLayer ResolveTargetLayer(const MouseButtonEvent& event) const noexcept;
	InputLayer ResolveTargetLayer(const MouseMoveEvent& event) const noexcept;
	InputLayer ResolveTargetLayer(const MouseWheelEvent& event) const noexcept;
	InputLayer GetActiveTargetLayer() const noexcept;
	void SetActiveTargetLayer(InputLayer layer) noexcept;
	void ResetFocus() noexcept;

private:
	struct TargetRegion final
	{
		float Left = 0.0f;
		float Top = 0.0f;
		float Right = 0.0f;
		float Bottom = 0.0f;
		InputLayer TargetLayer = InputLayer::UI;

		bool IsValid() const noexcept;
		bool Contains(MousePosition position) const noexcept;
	};

	InputLayer ResolvePointTarget(MousePosition position) const noexcept;
	std::vector<TargetRegion> m_targetRegions;
	InputLayer m_activeTargetLayer = InputLayer::Gameplay;
	bool m_bHasRoutingFrame = false;
	bool m_bInteractionDisabled = false;
	bool m_bTextInputActive = false;
};
