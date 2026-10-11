#pragma once

struct RenderFrame;

class FrameGraphBuilder;
class GpuMeshCache;
class RenderRayTracingScene;
struct RenderFrameGraphResources;
struct RenderFrameGraphSettings;

void AddRealTimePathTracerPasses(
    FrameGraphBuilder& builder,
    const RenderFrame& frame,
    const RenderFrameGraphSettings& settings,
    RenderRayTracingScene& rayTracingScene,
    GpuMeshCache& gpuMeshCache,
    RenderFrameGraphResources& resources);
