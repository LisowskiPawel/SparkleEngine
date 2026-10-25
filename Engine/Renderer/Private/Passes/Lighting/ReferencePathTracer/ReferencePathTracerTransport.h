#pragma once

struct RenderFrame;

#include "Renderer/Public/Viewport/ViewportContracts.h"

class FrameGraphBuilder;
class RenderRayTracingScene;
struct ReferencePathTracerGraphResources;
struct ReferencePathTracerWork;
struct RenderFrameGraphResources;

void AddReferencePathTracerTransportPass(
    FrameGraphBuilder& builder,
    const RenderFrame& frame,
    RenderViewportExtent extent,
    const ReferencePathTracerGraphResources& graphResources,
    const ReferencePathTracerWork& work,
    RenderRayTracingScene& rayTracingScene,
    const RenderFrameGraphResources& resources);
