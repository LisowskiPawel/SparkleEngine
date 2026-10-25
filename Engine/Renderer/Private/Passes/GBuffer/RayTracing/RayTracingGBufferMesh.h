#pragma once

struct RenderFrame;

#include "Renderer/Public/Viewport/ViewportContracts.h"

class FrameGraphBuilder;
class RenderRayTracingScene;
struct RenderFrameGraphResources;

void AddRayTracingGBufferMeshPass(
    FrameGraphBuilder& builder,
    const RenderFrame& frame,
    RenderViewportExtent sceneExtent,
    RenderRayTracingScene& rayTracingScene,
    const RenderFrameGraphResources& resources);
