#pragma once

#include "Pipeline/RhiPipelineService.h"

#include <memory>

template <typename BackendRhi, typename BackendPipeline, typename BackendRayTracingPipeline, auto CompileBindingLayout> class RhiPipelineServiceAdapter final : public RhiPipelineService
{
public:
	explicit RhiPipelineServiceAdapter(BackendRhi& rhi) noexcept :
	    m_rhi(rhi)
	{
	}

	std::unique_ptr<RenderBindingLayout> CreateBindingLayout(const RenderBindingLayoutCompileDesc& desc) override { return CompileBindingLayout(m_rhi, desc); }

	std::unique_ptr<RenderPipeline> CreateGraphicsPipeline(const GraphicsPipelineDesc& desc) override { return std::make_unique<BackendPipeline>(m_rhi, desc); }

	std::unique_ptr<RenderPipeline> CreateComputePipeline(const ComputePipelineDesc& desc) override { return std::make_unique<BackendPipeline>(m_rhi, desc); }

	std::unique_ptr<RayTracingPipeline> CreateRayTracingPipeline(const RayTracingPipelineDesc& desc) override { return std::make_unique<BackendRayTracingPipeline>(m_rhi, desc); }

private:
	BackendRhi& m_rhi;
};
