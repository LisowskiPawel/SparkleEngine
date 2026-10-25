#pragma once

#include "Pipeline/PassBinder.h"

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
    std::span<const char* const> bindingNames,
    const PassBindingOverrides* overrides,
    bool bindLayout,
    BindingDomain domain);
