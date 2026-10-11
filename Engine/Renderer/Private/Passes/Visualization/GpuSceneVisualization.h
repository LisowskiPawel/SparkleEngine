#pragma once

#include "Renderer/Public/Viewport/ViewportContracts.h"

class FrameGraphBuilder;
struct RenderFrameGraphResources;

bool IsGpuSceneVisualizationActive(RenderViewMode viewMode) noexcept;

void AddGpuSceneVisualizationPass(FrameGraphBuilder& builder, RenderViewportExtent sceneExtent, const RenderFrameGraphResources& resources);
