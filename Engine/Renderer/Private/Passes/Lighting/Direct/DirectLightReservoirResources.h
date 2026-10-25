#pragma once

#include "Renderer/Public/Viewport/ViewportContracts.h"

class FrameGraphBuilder;
struct RenderFrameGraphResources;

void CreateDirectLightReservoirResources(FrameGraphBuilder& builder, RenderViewportExtent extent, RenderFrameGraphResources& resources);
