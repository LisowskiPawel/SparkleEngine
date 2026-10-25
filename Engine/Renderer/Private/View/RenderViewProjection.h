#pragma once

#include "Renderer/Public/Viewport/ViewportContracts.h"

struct RenderView;

void BuildRenderViewCamera(RenderView& view, RenderViewportExtent projectionExtent) noexcept;
void BuildRenderViewRasterState(RenderView& view, RenderViewportExtent extent) noexcept;
