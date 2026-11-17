#pragma once

#include "Pipeline/RhiPipelineService.h"

#include "Shaders/ResolvedShader.h"
#include "Core/Public/Diagnostics/Logger.h"
#include "Core/Public/Diagnostics/Verify.h"

#include <memory>

template <typename BackendRhi, typename BackendPipeline, typename BackendRayTracingPipeline, auto CompileBindingLayout> class RhiPipelineServiceAdapter final : public RhiPipelineService
{
public:
	explicit RhiPipelineServiceAdapter(BackendRhi& rhi) noexcept :
	    m_rhi(rhi)
	{
	}

	std::unique_ptr<RenderBindingLayout> CreateBindingLayout(const RenderBindingLayoutCompileDesc& desc) override
	{
		if (desc.ParameterLayout == nullptr || desc.Shaders.empty())
		{
			Fail("Binding-layout creation received an incomplete compile description.");
		}
		return CompileBindingLayout(m_rhi, desc);
	}

	std::unique_ptr<RenderPipeline> CreateGraphicsPipeline(const GraphicsPipelineDesc& desc) override
	{
		const bool bShaderValid = desc.VertexShader != nullptr && desc.VertexShader->IsValid();
		if (desc.BindingLayout == nullptr || !bShaderValid)
		{
			Fail("Graphics-pipeline creation received an incomplete pipeline description.");
		}
		return std::make_unique<BackendPipeline>(m_rhi, desc);
	}

	std::unique_ptr<RenderPipeline> CreateComputePipeline(const ComputePipelineDesc& desc) override
	{
		const bool bShaderValid = desc.ComputeShader != nullptr && desc.ComputeShader->IsValid();
		if (desc.BindingLayout == nullptr || !bShaderValid)
		{
			Fail("Compute-pipeline creation received an incomplete pipeline description.");
		}
		return std::make_unique<BackendPipeline>(m_rhi, desc);
	}

	std::unique_ptr<RayTracingPipeline> CreateRayTracingPipeline(const RayTracingPipelineDesc& desc) override
	{
		if (desc.GlobalBindingLayout == nullptr || desc.ShaderExports.empty() || desc.Generation == 0)
		{
			Fail("Ray-tracing pipeline creation received an incomplete pipeline description.");
		}
		return std::make_unique<BackendRayTracingPipeline>(m_rhi, desc);
	}

private:
	[[noreturn]] static void Fail(const char* message)
	{
		SPARKLE_DEFINE_LOG_CATEGORY_STATIC(logger, "RHI.Pipeline");
		Diagnostics::Fatal(logger, __FILE__, __LINE__, message);
	}

	BackendRhi& m_rhi;
};
