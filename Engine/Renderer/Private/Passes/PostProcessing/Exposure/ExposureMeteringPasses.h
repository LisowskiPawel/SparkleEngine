#pragma once

#include "Passes/PostProcessing/Exposure/ExposureMeteringResources.h"
#include "Renderer/Public/Viewport/ViewportContracts.h"

class FrameGraphBuilder;
struct RenderFrameGraphResources;

ExposureMomentTexture AddExposureHistogramPasses(FrameGraphBuilder& builder, RenderViewportExtent sceneExtent, const RenderFrameGraphResources& resources);
ExposureMomentTexture AddExposureDownsamplePasses(FrameGraphBuilder& builder, RenderViewportExtent sceneExtent, const RenderFrameGraphResources& resources);
