#include "../../PCH.h"
#include "Passes/Core/ShaderPass.h"

#include "FrameGraph/Builder/PassResourceBuilder.h"
#include "FrameGraph/Execution/FrameGraphResourceCommands.h"
#include "Commands/RenderCommandContext.h"
#include "ShaderParameters/PassParameterSet.h"

#include "RHI/Public/ShaderParameters/PassParameterLayout.h"

#include "Pipeline/PassBinder.h"

#include <cassert>

void DispatchComputeShaderPass(RenderCommandContext& commandContext, const ComputeDispatchDesc& dispatch) noexcept
{
	assert(dispatch.GroupCountX > 0);
	assert(dispatch.GroupCountY > 0);
	assert(dispatch.GroupCountZ > 0);
	commandContext.Dispatch(dispatch.GroupCountX, dispatch.GroupCountY, dispatch.GroupCountZ);
}

void BindComputeShaderPass(
    RenderCommandList& commandList,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& bindingLayout,
    const RenderPipeline& pipeline,
    const PassParameterSet& parameterSet,
    const PassBindingOverrides* overrides) noexcept
{
	resources.BindGlobalDescriptorState(commandList);

	commandList.SetPipeline(pipeline);

	BindPassParameters(commandList, resources, bindingLayout, parameterSet, BindingDomain::Compute, overrides);
}

void BindRasterShaderPass(
    RenderCommandList& commandList,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& bindingLayout,
    const RenderPipeline& pipeline,
    const PassParameterSet& parameterSet,
    const PassBindingOverrides* overrides) noexcept
{
	resources.BindGlobalDescriptorState(commandList);

	commandList.SetPipeline(pipeline);

	BindPassParameters(commandList, resources, bindingLayout, parameterSet, BindingDomain::Graphics, overrides);
}

void BindRayTracingShaderPass(
    RenderCommandList& commandList,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& bindingLayout,
    const RayTracingPipeline& pipeline,
    const PassParameterSet& parameterSet) noexcept
{
	resources.BindGlobalDescriptorState(commandList);
	commandList.SetRayTracingPipeline(pipeline);
	BindPassParameters(commandList, resources, bindingLayout, parameterSet, BindingDomain::RayTracing);
}
