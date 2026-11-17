#pragma once

#include "Pipeline/PassBindingOverrides.h"
#include "RHI/Public/Bindings/RenderBindingLayout.h"

#include <span>

class RenderCommandContext;
class FrameGraphResourceCommands;
struct PassParameterBinding;
class PassParameterSet;

void BindGraphicsPassParameters(
    RenderCommandContext& commandContext,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& layout,
    const PassParameterSet& parameterSet,
    std::span<const char* const> bindingNames = {},
    const PassBindingOverrides* overrides = nullptr,
    bool bBindLayout = true);

void BindComputePassParameters(
    RenderCommandContext& commandContext,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& layout,
    const PassParameterSet& parameterSet,
    std::span<const char* const> bindingNames = {},
    const PassBindingOverrides* overrides = nullptr,
    bool bBindLayout = true);
void BindRayTracingPassParameters(
    RenderCommandContext& commandContext,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& layout,
    const PassParameterSet& parameterSet,
    std::span<const char* const> bindingNames = {},
    const PassBindingOverrides* overrides = nullptr,
    bool bBindLayout = true);
