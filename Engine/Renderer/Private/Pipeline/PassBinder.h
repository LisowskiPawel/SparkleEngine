#pragma once

#include "Pipeline/PassBindingOverrides.h"
#include "RHI/Public/Pipeline/RhiPipelineDesc.h"

#include <cstdint>
#include <span>
#include <string_view>

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
    bool bindLayout = true);

void BindComputePassParameters(
    RenderCommandContext& commandContext,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& layout,
    const PassParameterSet& parameterSet,
    std::span<const char* const> bindingNames = {},
    const PassBindingOverrides* overrides = nullptr,
    bool bindLayout = true);
void BindRayTracingPassParameters(
    RenderCommandContext& commandContext,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& layout,
    const PassParameterSet& parameterSet,
    std::span<const char* const> bindingNames = {},
    const PassBindingOverrides* overrides = nullptr,
    bool bindLayout = true);
