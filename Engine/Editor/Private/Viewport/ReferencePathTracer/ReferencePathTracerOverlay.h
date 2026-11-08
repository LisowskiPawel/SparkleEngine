#pragma once

#include "Viewport/ViewportOutputAction.h"
#include "Renderer/Public/Viewport/ViewportContracts.h"

#include <optional>

struct ReferencePathTracerOverlayActions final
{
	std::optional<RenderViewMode> ViewMode;
	ViewportRenderAction RenderAction = ViewportRenderAction::None;
	ViewportOutputAction OutputAction = ViewportOutputAction::None;
};

ReferencePathTracerOverlayActions DrawReferencePathTracerOverlay(const ViewportRenderProgress& progress) noexcept;
