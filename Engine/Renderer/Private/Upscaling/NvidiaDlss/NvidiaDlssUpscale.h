#pragma once

#include "Renderer/Public/Viewport/ViewportContracts.h"

class FrameGraphBuilder;
class RendererImageProviderStack;
struct RenderFrameGraphResources;

void AddNvidiaDlssUpscalePass(
    FrameGraphBuilder& builder,
    RenderViewportExtent renderExtent,
    RenderViewportExtent outputExtent,
    RendererImageProviderStack& imageProviders,
    const RenderFrameGraphResources& resources);
