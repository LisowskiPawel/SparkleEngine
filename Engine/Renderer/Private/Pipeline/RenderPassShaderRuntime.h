#pragma once

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Strings/StringUtils.h"
#include "Pipeline/PipelineMaterialization.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"
#include "RHI/Public/ShaderParameters/PassParameterLayout.h"
#include "RHI/Public/Shaders/ShaderParameterLayoutBuilder.h"

#include <array>
#include <format>
#include <memory>
#include <string>
#include <vector>

struct RenderPassShaderRuntimeStorage
{
	PassParameterLayout ParameterLayout;
	std::vector<ResolvedShader> Shaders;
	std::unique_ptr<RenderBindingLayout> BindingLayout;
};

namespace RenderPassShaderRuntime
{

	template <typename TVertexShader, typename TPixelShader> void CreateGraphicsRuntime(
	    RenderHardwareInterface& renderHardwareInterface,
	    const ShaderRegistrationDesc& vertexRegistration,
	    ShaderRef<TVertexShader> vertexShader,
	    const ShaderRegistrationDesc& pixelRegistration,
	    ShaderRef<TPixelShader> pixelShader,
	    RenderPassShaderRuntimeStorage& storage)
	{
		if (!vertexShader || !pixelShader)
		{
			throw Diagnostics::Error("Graphics shader types did not resolve through the active map.");
		}
		const ShaderRegistrationDesc* registrations[] = {&vertexRegistration, &pixelRegistration};
		storage.ParameterLayout = BuildShaderPipelineParameterLayout(registrations);
		storage.Shaders = {vertexShader.GetResolvedShader(), pixelShader.GetResolvedShader()};
		PipelineMaterialization::ValidateResolvedShader(renderHardwareInterface, vertexRegistration, storage.Shaders[0], BuildShaderParameterLayout(vertexRegistration));
		PipelineMaterialization::ValidateResolvedShader(renderHardwareInterface, pixelRegistration, storage.Shaders[1], BuildShaderParameterLayout(pixelRegistration));
		std::wstring debugName = Strings::ToWide(vertexRegistration.ShaderName);
		storage.BindingLayout = PipelineMaterialization::CreateBindingLayout(renderHardwareInterface, storage.ParameterLayout, storage.Shaders, true, debugName.c_str());
	}

	template <typename TShader> void CreateComputeRuntime(
	    RenderHardwareInterface& renderHardwareInterface,
	    const ShaderRegistrationDesc& registration,
	    ShaderRef<TShader> shader,
	    RenderPassShaderRuntimeStorage& storage,
	    std::unique_ptr<RenderPipeline>& pipelineStorage)
	{
		if (!shader)
		{
			throw Diagnostics::Error(std::format("Shader '{}' did not resolve through the active map.", registration.ShaderName));
		}
		storage.ParameterLayout = BuildShaderParameterLayout(registration);
		storage.Shaders = {shader.GetResolvedShader()};
		PipelineMaterialization::ValidateResolvedShader(renderHardwareInterface, registration, storage.Shaders.front(), storage.ParameterLayout);
		std::wstring debugName = Strings::ToWide(registration.ShaderName);
		storage.BindingLayout = PipelineMaterialization::CreateBindingLayout(renderHardwareInterface, storage.ParameterLayout, storage.Shaders, false, debugName.c_str());
		ComputePipelineDesc pipeline;
		pipeline.BindingLayout = storage.BindingLayout.get();
		pipeline.ComputeShader = RhiShaderStageDesc{&storage.Shaders.front()};
		pipeline.DebugName = debugName.c_str();
		pipelineStorage = PipelineMaterialization::CreateComputePipeline(renderHardwareInterface, pipeline);
	}
}
