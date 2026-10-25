#include "../../PCH.h"
#include "Passes/Core/ShaderPass.h"

#include "FrameGraph/Builder/PassResourceBuilder.h"
#include "FrameGraph/Execution/FrameGraphResourceCommands.h"
#include "Commands/RenderCommandContext.h"
#include "ShaderParameters/PassParameterSet.h"

#include "RHI/Public/ShaderParameters/PassParameterLayout.h"

#include "Core/Public/Diagnostics/Logger.h"
#include "Core/Public/Diagnostics/Verify.h"
#include "Pipeline/PassBinder.h"

#include <cassert>
#include <string>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_shaderPassLogger, "Renderer");

bool DeclareShaderPassParameterUsages(PassResourceBuilder& builder, const PassParameterSet& parameterSet, const char* passName) noexcept
{
	return builder.DeclareParameterUsages(parameterSet, passName != nullptr ? passName : "");
}

void DispatchComputeShaderPass(RenderCommandContext& commandContext, const ComputeDispatchDesc& dispatch) noexcept
{
	assert(dispatch.GroupCountX > 0);
	assert(dispatch.GroupCountY > 0);
	assert(dispatch.GroupCountZ > 0);
	commandContext.Dispatch(dispatch.GroupCountX, dispatch.GroupCountY, dispatch.GroupCountZ);
}

static bool HasBindingOverride(const PassBindingOverrides* overrides, const char* bindingName) noexcept
{
	if (overrides == nullptr || bindingName == nullptr)
	{
		return false;
	}

	for (const PassBindingOverride& bindingOverride : overrides->GetOverrides())
	{
		if (bindingOverride.Name == bindingName)
		{
			return true;
		}
	}

	return false;
}

bool ValidateShaderParameterSetup(const PassParameterSet& parameterSet, const char* passName) noexcept
{
	if (!parameterSet.HasLayout())
	{
		ReportInvalidShaderPassParameterSet(passName, parameterSet);
		return false;
	}

	const PassParameterLayout* layout = parameterSet.GetLayout();
	if (layout == nullptr)
	{
		ReportInvalidShaderPassParameterSet(passName, parameterSet);
		return false;
	}

	const std::vector<PassParameterDesc>& parameters = layout->GetParameters();
	for (std::uint32_t index = 0; index < static_cast<std::uint32_t>(parameters.size()); ++index)
	{
		if (!parameterSet.UsesGraphResource(index))
		{
			continue;
		}

		const PassParameterBinding* binding = parameterSet.GetBinding(index);
		if (binding == nullptr || !binding->IsBound())
		{
			ReportInvalidShaderPassParameterSet(passName, parameterSet);
			return false;
		}
	}

	return true;
}

bool ValidateShaderParameters(
    const PassParameterSet& parameterSet,
    const char* passName,
    const char* const* bindingNames,
    std::uint32_t bindingNameCount,
    const PassBindingOverrides* overrides) noexcept
{
	if (!parameterSet.HasLayout())
	{
		ReportInvalidShaderPassParameterSet(passName, parameterSet);
		return false;
	}

	if (bindingNames != nullptr && bindingNameCount > 0)
	{
		for (std::uint32_t index = 0; index < bindingNameCount; ++index)
		{
			const PassParameterBinding* binding = parameterSet.FindBinding(bindingNames[index]);
			if ((binding == nullptr || !binding->IsBound()) && !HasBindingOverride(overrides, bindingNames[index]))
			{
				ReportInvalidShaderPassParameterSet(passName, parameterSet, bindingNames[index]);
				return false;
			}
		}
		return true;
	}

	if (!parameterSet.HasAllRequiredBindings())
	{
		ReportInvalidShaderPassParameterSet(passName, parameterSet);
		return false;
	}

	return true;
}

void BindComputeShaderPass(
    RenderCommandContext& commandContext,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& bindingLayout,
    const RenderPipeline& pipeline,
    const PassParameterSet& parameterSet,
    const char* const* bindingNames,
    std::uint32_t bindingNameCount,
    const PassBindingOverrides* overrides,
    bool bindLayout) noexcept
{
	if (bindLayout)
	{
		resources.BindGlobalDescriptorState(commandContext);
	}

	commandContext.SetPipeline(pipeline);

	BindComputePassParameters(
	    commandContext,
	    resources,
	    bindingLayout,
	    parameterSet,
	    bindingNames != nullptr ? std::span<const char* const>(bindingNames, bindingNameCount) : std::span<const char* const>{},
	    overrides,
	    bindLayout);
}

void BindRasterShaderPass(
    RenderCommandContext& commandContext,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& bindingLayout,
    const RenderPipeline& pipeline,
    const PassParameterSet& parameterSet,
    const char* const* bindingNames,
    std::uint32_t bindingNameCount,
    const PassBindingOverrides* overrides,
    bool bindLayout) noexcept
{
	if (bindLayout)
	{
		resources.BindGlobalDescriptorState(commandContext);
	}

	commandContext.SetPipeline(pipeline);

	BindGraphicsPassParameters(
	    commandContext,
	    resources,
	    bindingLayout,
	    parameterSet,
	    bindingNames != nullptr ? std::span<const char* const>(bindingNames, bindingNameCount) : std::span<const char* const>{},
	    overrides,
	    bindLayout);
}

void BindRayTracingShaderPass(
    RenderCommandContext& commandContext,
    const FrameGraphResourceCommands& resources,
    const RenderBindingLayout& bindingLayout,
    const RayTracingPipeline& pipeline,
    const PassParameterSet& parameterSet) noexcept
{
	resources.BindGlobalDescriptorState(commandContext);
	commandContext.SetRayTracingPipeline(pipeline);
	BindRayTracingPassParameters(commandContext, resources, bindingLayout, parameterSet);
}

[[noreturn]] void ReportInvalidShaderPassParameterSet(const char* passName, const PassParameterSet& parameterSet, const char* requiredBinding) noexcept
{
	std::string message = "Shader pass parameter validation failed";
	if (passName != nullptr && passName[0] != '\0')
	{
		message += " for pass '";
		message += passName;
		message += "'";
	}

	if (requiredBinding != nullptr)
	{
		message += ": required binding '";
		message += requiredBinding;
		message += "' is absent or unbound.";
		Diagnostics::Fatal(g_shaderPassLogger, __FILE__, __LINE__, message);
	}

	if (!parameterSet.HasLayout())
	{
		message += ": missing parameter layout.";
		Diagnostics::Fatal(g_shaderPassLogger, __FILE__, __LINE__, message);
	}

	const std::vector<std::string> missingBindings = parameterSet.GetMissingBindings();
	if (missingBindings.empty())
	{
		message += ": parameter bindings are incomplete.";
		Diagnostics::Fatal(g_shaderPassLogger, __FILE__, __LINE__, message);
	}

	message += ": missing bindings ";
	for (std::size_t index = 0; index < missingBindings.size(); ++index)
	{
		if (index > 0)
		{
			message += ", ";
		}

		message += missingBindings[index];
	}
	message += ".";
	Diagnostics::Fatal(g_shaderPassLogger, __FILE__, __LINE__, message);
}
