#pragma once

#include "RHI/Public/Pipeline/RhiPipelineDesc.h"

#include <memory>
#include <span>
#include <string_view>

class PassParameterLayout;
class RenderHardwareInterface;
struct ShaderRegistrationDesc;

namespace PipelineMaterialization
{
	void ValidateResolvedShader(RenderHardwareInterface& renderHardwareInterface, const ShaderRegistrationDesc& registration, const ResolvedShader& shader, const PassParameterLayout& parameterLayout);

	void ValidateShaderCapabilities(RenderHardwareInterface& renderHardwareInterface, std::string_view shaderName, const ResolvedShader& shader);

	std::unique_ptr<RenderBindingLayout> CreateBindingLayout(
	    RenderHardwareInterface& renderHardwareInterface,
	    const PassParameterLayout& parameterLayout,
	    std::span<const ResolvedShader> shaders,
	    bool bAllowInputAssemblerInputLayout,
	    const wchar_t* debugName);

	std::unique_ptr<RenderPipeline> CreateGraphicsPipeline(RenderHardwareInterface& renderHardwareInterface, const GraphicsPipelineDesc& pipelineDesc);

	std::unique_ptr<RenderPipeline> CreateComputePipeline(RenderHardwareInterface& renderHardwareInterface, const ComputePipelineDesc& pipelineDesc);
}
