#pragma once

#include "RayTracing/Acceleration/RayTracingTlasBuilder.h"
#include "RayTracing/Acceleration/RayTracingAccelerationStrategy.h"

class RenderHardwareInterface;

class RayTracingTlasStrategy final : public RayTracingAccelerationStrategy
{
public:
	explicit RayTracingTlasStrategy(RenderHardwareInterface& renderHardwareInterface) noexcept;
	~RayTracingTlasStrategy() noexcept override;
	RenderRayTracingFrameBindings Prepare(const PreparedRenderScene& preparedScene, const RayTracingPtlasPartitionPlan& viewPlan) noexcept override;
	RayTracingTlasBuildResult Build(
	    RenderCommandContext& commandContext,
	    const PreparedRenderScene& preparedScene,
	    RayTracingBlasCache& blasCache,
	    const RayTracingShaderTablePlan& shaderTablePlan,
	    const RayTracingPtlasPartitionPlan& viewPlan,
	    RayTracingPerformanceDiagnostics* diagnostics) noexcept override;
	void Clear() noexcept override;

private:
	RayTracingTlasBuilder m_tlasBuilder;
};
