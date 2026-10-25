#pragma once

#include "RayTracing/Acceleration/RayTracingClassicTlasBuilder.h"
#include "RayTracing/Acceleration/RayTracingTopLevelAccelerationStructureStrategy.h"

class RenderHardwareInterface;

class RayTracingClassicTlasStrategy final : public RayTracingTopLevelAccelerationStructureStrategy
{
public:
	explicit RayTracingClassicTlasStrategy(RenderHardwareInterface& renderHardwareInterface) noexcept;
	~RayTracingClassicTlasStrategy() noexcept override;
	RenderRayTracingFrameBindings Prepare(const PreparedRenderScene& preparedScene, const RayTracingPtlasPartitionPlan& viewPlan) noexcept override;
	RayTracingTopLevelAccelerationStructureBuildResult Build(
	    RenderCommandContext& commandContext,
	    const PreparedRenderScene& preparedScene,
	    RayTracingBlasCache& blasCache,
	    const RayTracingShaderTablePlan& shaderTablePlan,
	    const RayTracingPtlasPartitionPlan& viewPlan,
	    RayTracingPerformanceDiagnostics* diagnostics) noexcept override;
	void Clear() noexcept override;

private:
	RayTracingClassicTlasBuilder m_classicTlasBuilder;
};
