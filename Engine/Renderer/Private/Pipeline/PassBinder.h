#pragma once

#include "Pipeline/PassBindingOverrides.h"
#include "RHI/Public/Bindings/RenderBindingLayout.h"

#include <cstdint>

class RenderCommandContext;
class FrameGraphResourceCommands;
class PassParameterSet;

enum class BindingDomain : std::uint8_t
{
	Graphics,
	Compute,
	RayTracing,
};

void BindPassParameters(
    RenderCommandContext& commandContext,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& layout,
    const PassParameterSet& parameterSet,
    BindingDomain domain,
    const PassBindingOverrides* overrides = nullptr);
