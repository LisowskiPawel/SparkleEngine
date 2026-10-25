#pragma once
#include "FrameGraph/FrameGraphTextureHandle.h"
#include <span>
#include <string_view>

class FrameGraphBuilder;
struct RenderViewportExtent;
struct RenderFrameGraphResources;

void AddLightingTargetClearPass(FrameGraphBuilder& builder, std::string_view name, RenderViewportExtent extent, std::span<const FrameGraphTextureHandle> targets);

void AddDirectLightingClearPass(FrameGraphBuilder& builder, RenderViewportExtent extent, const RenderFrameGraphResources& resources);
void AddIndirectLightingClearPass(FrameGraphBuilder& builder, RenderViewportExtent extent, const RenderFrameGraphResources& resources);
void AddRayReconstructionGuideClearPass(FrameGraphBuilder& builder, RenderViewportExtent extent, const RenderFrameGraphResources& resources);
