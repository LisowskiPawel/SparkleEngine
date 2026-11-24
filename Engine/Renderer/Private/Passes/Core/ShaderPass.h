#pragma once

#include "FrameGraph/Builder/PassResourceBuilder.h"
#include "Pipeline/PassBindingOverrides.h"
#include "ShaderParameters/PassParameterSet.h"

#include <cstdint>
#include <type_traits>

class RenderCommandContext;
class RenderCommandList;
class FrameGraphResourceCommands;
class RenderBindingLayout;
class RenderPipeline;
class RayTracingPipeline;

struct ComputeDispatchDesc
{
	std::uint32_t GroupCountX = 1;
	std::uint32_t GroupCountY = 1;
	std::uint32_t GroupCountZ = 1;
};

struct RayTracingDispatchDimensions final
{
	std::uint32_t Width = 1;
	std::uint32_t Height = 1;
	std::uint32_t Depth = 1;
};

void DispatchComputeShaderPass(RenderCommandContext& commandContext, const ComputeDispatchDesc& dispatch) noexcept;
void BindComputeShaderPass(
    RenderCommandList& commandList,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& bindingLayout,
    const RenderPipeline& pipeline,
    const PassParameterSet& parameterSet,
    const PassBindingOverrides* overrides = nullptr) noexcept;

void BindRasterShaderPass(
    RenderCommandList& commandList,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& bindingLayout,
    const RenderPipeline& pipeline,
    const PassParameterSet& parameterSet,
    const PassBindingOverrides* overrides = nullptr) noexcept;
void BindRayTracingShaderPass(
    RenderCommandList& commandList,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& bindingLayout,
    const RayTracingPipeline& pipeline,
    const PassParameterSet& parameterSet) noexcept;

template <typename TParameterBindings> void SetupShaderParameters(PassResourceBuilder& builder, const TParameterBindings& parameters) noexcept
{
	if constexpr (std::is_same_v<std::remove_cvref_t<TParameterBindings>, PassParameterSet>)
	{
		builder.DeclareParameterUsages(parameters);
	}
	else
	{
		builder.DeclareParameterUsages(parameters.GetPassParameterSet());
	}
}
