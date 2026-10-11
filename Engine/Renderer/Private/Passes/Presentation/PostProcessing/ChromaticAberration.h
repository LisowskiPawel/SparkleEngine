#pragma once

#include "Core/Public/Console/CVar.h"
#include "FrameGraph/FrameGraphTextureHandle.h"
#include "Renderer/Public/Viewport/ViewportContracts.h"

class FrameGraphBuilder;

extern ConsoleVariable<bool> CVarChromaticAberration;
bool IsChromaticAberrationActive(RenderViewMode viewMode) noexcept;

FrameGraphTextureHandle AddChromaticAberrationPass(FrameGraphBuilder& builder, RenderViewMode viewMode, RenderViewportExtent outputExtent, FrameGraphTextureHandle input);
