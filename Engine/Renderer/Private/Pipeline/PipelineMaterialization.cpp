#include "../PCH.h"

#include "PipelineMaterialization.h"

#include "Core/Public/Diagnostics/Error.h"
#include "RHI/Public/Core/RhiBackendSelection.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"
#include "RHI/Public/ShaderParameters/PassParameterLayout.h"
#include "RHI/Public/Shaders/Authoring/GlobalShader.h"

#include <format>

void PipelineMaterialization::ValidateShaderCapabilities(RenderHardwareInterface& renderHardwareInterface, std::string_view shaderName, const ResolvedShader& shader)
{
	if (!shader.IsValid())
	{
		throw Diagnostics::Error(std::format("Shader '{}' did not resolve through the active global shader map.", shaderName));
	}
	const RhiCapabilities& capabilities = renderHardwareInterface.GetCapabilities();
	const bool bAccelerationStructuresUnsupported = HasShaderFeature(shader.Entry->Features, ShaderFeatureFlags::UsesAccelerationStructure) && !capabilities.RayTracing.bSupportsAccelerationStructure;

	if (bAccelerationStructuresUnsupported)
	{
		throw Diagnostics::Error(std::format("Shader '{}' requires ray tracing, but backend '{}' does not support it.", shaderName, RhiBackendApiToString(capabilities.BackendApi)));
	}
	const bool bInlineRayQueriesUnsupported = HasShaderFeature(shader.Entry->Features, ShaderFeatureFlags::UsesInlineRayQuery) && !capabilities.RayTracing.bSupportsInlineRayQuery;

	if (bInlineRayQueriesUnsupported)
	{
		throw Diagnostics::Error(std::format("Shader '{}' requires inline ray query, but backend '{}' does not support it.", shaderName, RhiBackendApiToString(capabilities.BackendApi)));
	}
	const bool bIndexingUnsupportedEntry = HasShaderFeature(shader.Entry->Features, ShaderFeatureFlags::UsesDescriptorIndexing);
	const bool bIndexingUnsupportedIndexing = !capabilities.DescriptorIndexing.bNonUniformSampledImages || !capabilities.DescriptorIndexing.bSupportsPartiallyBoundDescriptorArrays;
	const bool bDescriptorIndexingUnsupported = bIndexingUnsupportedEntry && bIndexingUnsupportedIndexing;

	if (bDescriptorIndexingUnsupported)
	{
		throw Diagnostics::Error(std::format("Shader '{}' requires descriptor indexing, but backend '{}' does not support it.", shaderName, RhiBackendApiToString(capabilities.BackendApi)));
	}
	if (HasShaderFeature(shader.Entry->Features, ShaderFeatureFlags::UsesFloat64) && !capabilities.bSupportsShaderFloat64)
	{
		throw Diagnostics::Error(std::format("Shader '{}' requires float64 arithmetic, but backend '{}' does not support it.", shaderName, RhiBackendApiToString(capabilities.BackendApi)));
	}
}

void PipelineMaterialization::ValidateResolvedShader(
    RenderHardwareInterface& renderHardwareInterface,
    const ShaderRegistrationDesc& registration,
    const ResolvedShader& shader,
    const PassParameterLayout& parameterLayout)
{
	PipelineMaterialization::ValidateShaderCapabilities(renderHardwareInterface, registration.ShaderName, shader);
	const bool bShaderContractMismatchFeatures = shader.Entry->Stage != registration.Stage || shader.Entry->Features != registration.Features;
	const bool bShaderContractMismatch = bShaderContractMismatchFeatures || (shader.Entry->ParameterSignature != BuildShaderParameterSignature(parameterLayout));

	if (bShaderContractMismatch)
	{
		throw Diagnostics::Error(std::format("Shader '{}' map metadata does not match its registered contract.", registration.ShaderName));
	}
}

std::unique_ptr<RenderBindingLayout> PipelineMaterialization::CreateBindingLayout(
    RenderHardwareInterface& renderHardwareInterface,
    const PassParameterLayout& parameterLayout,
    std::span<const ResolvedShader> shaders,
    bool bAllowInputAssemblerInputLayout,
    const wchar_t* debugName)
{
	RenderBindingLayoutCompileDesc desc;
	desc.ParameterLayout = &parameterLayout;
	desc.Shaders = shaders;
	desc.bAllowInputAssemblerInputLayout = bAllowInputAssemblerInputLayout;
	desc.DebugName = debugName;
	std::unique_ptr<RenderBindingLayout> layout = renderHardwareInterface.GetPipelineService().CreateBindingLayout(desc);
	if (!layout)
	{
		throw Diagnostics::Error("Shader binding-layout creation failed.");
	}
	return layout;
}

std::unique_ptr<RenderPipeline> PipelineMaterialization::CreateGraphicsPipeline(RenderHardwareInterface& renderHardwareInterface, const GraphicsPipelineDesc& pipelineDesc)
{
	std::unique_ptr<RenderPipeline> pipeline = renderHardwareInterface.GetPipelineService().CreateGraphicsPipeline(pipelineDesc);
	if (!pipeline)
	{
		throw Diagnostics::Error("Graphics pipeline creation failed.");
	}
	return pipeline;
}

std::unique_ptr<RenderPipeline> PipelineMaterialization::CreateComputePipeline(RenderHardwareInterface& renderHardwareInterface, const ComputePipelineDesc& pipelineDesc)
{
	std::unique_ptr<RenderPipeline> pipeline = renderHardwareInterface.GetPipelineService().CreateComputePipeline(pipelineDesc);
	if (!pipeline)
	{
		throw Diagnostics::Error("Compute pipeline creation failed.");
	}
	return pipeline;
}
