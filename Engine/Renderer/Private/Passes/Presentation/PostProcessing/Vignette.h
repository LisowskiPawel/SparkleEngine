#pragma once

#include "Core/Public/Console/CVar.h"
#include "FrameGraph/FrameGraphTextureHandle.h"
#include "Renderer/Public/Viewport/ViewportContracts.h"

class FrameGraphBuilder;

extern ConsoleVariable<bool> CVarVignette;
bool IsVignetteActive(RenderViewMode viewMode) noexcept;

FrameGraphTextureHandle AddVignettePass(FrameGraphBuilder& builder, RenderViewMode viewMode, RenderViewportExtent outputExtent, FrameGraphTextureHandle input);
