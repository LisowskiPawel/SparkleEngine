#pragma once

#include "Renderer/Public/Viewport/ViewportContracts.h"

class FrameGraphBuilder;
struct ReferencePathTracerGraphResources;
struct ReferencePathTracerWork;
struct RenderFrameGraphResources;

void AddReferencePathTracerDisplayPass(
    FrameGraphBuilder& builder,
    RenderViewportExtent extent,
    const ReferencePathTracerGraphResources& graphResources,
    const ReferencePathTracerWork& work,
    const RenderFrameGraphResources& resources);
