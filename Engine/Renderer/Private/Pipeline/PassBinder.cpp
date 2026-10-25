#include "../PCH.h"
#include "Pipeline/PassBinder.h"

#include "Pipeline/PassBindingOperations.h"

void BindGraphicsPassParameters(
    RenderCommandContext& commandContext,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& layout,
    const PassParameterSet& parameterSet,
    std::span<const char* const> bindingNames,
    const PassBindingOverrides* overrides,
    bool bindLayout)
{
	BindPassParameters(commandContext, resources, layout, parameterSet, bindingNames, overrides, bindLayout, BindingDomain::Graphics);
}

void BindComputePassParameters(
    RenderCommandContext& commandContext,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& layout,
    const PassParameterSet& parameterSet,
    std::span<const char* const> bindingNames,
    const PassBindingOverrides* overrides,
    bool bindLayout)
{
	BindPassParameters(commandContext, resources, layout, parameterSet, bindingNames, overrides, bindLayout, BindingDomain::Compute);
}

void BindRayTracingPassParameters(
    RenderCommandContext& commandContext,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& layout,
    const PassParameterSet& parameterSet,
    std::span<const char* const> bindingNames,
    const PassBindingOverrides* overrides,
    bool bindLayout)
{
	BindPassParameters(commandContext, resources, layout, parameterSet, bindingNames, overrides, bindLayout, BindingDomain::RayTracing);
}
