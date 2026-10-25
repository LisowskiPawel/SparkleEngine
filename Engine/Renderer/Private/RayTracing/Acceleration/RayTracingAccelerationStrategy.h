#pragma once

#include "Scene/RayTracing/RenderRayTracingFrameBindings.h"
#include "RHI/Public/RayTracing/RhiRayTracingDesc.h"

#include <cstdint>
#include <memory>

class RayTracingBlasCache;
class RayTracingPerformanceDiagnostics;
class RayTracingShaderTablePlan;
class RenderCommandContext;
class RenderHardwareInterface;
struct RayTracingCapabilityReport;
struct PreparedRenderScene;
struct RayTracingPtlasPartitionPlan;

struct RayTracingTlasBuildResult final
{
	std::uint32_t InstanceCount = 0;
	ERhiRayTracingTlasProvider ActiveProvider = ERhiRayTracingTlasProvider::None;
	const char* ActiveProviderReason = "not-built";
};

class RayTracingAccelerationStrategy
{
public:
	virtual ~RayTracingAccelerationStrategy() noexcept;

	RayTracingAccelerationStrategy(const RayTracingAccelerationStrategy&) = delete;
	RayTracingAccelerationStrategy& operator=(const RayTracingAccelerationStrategy&) = delete;
	RayTracingAccelerationStrategy(RayTracingAccelerationStrategy&&) = delete;
	RayTracingAccelerationStrategy& operator=(RayTracingAccelerationStrategy&&) = delete;
	virtual RenderRayTracingFrameBindings Prepare(const PreparedRenderScene& preparedScene, const RayTracingPtlasPartitionPlan& viewPlan) noexcept = 0;

	virtual RayTracingTlasBuildResult Build(
	    RenderCommandContext& commandContext,
	    const PreparedRenderScene& preparedScene,
	    RayTracingBlasCache& blasCache,
	    const RayTracingShaderTablePlan& shaderTablePlan,
	    const RayTracingPtlasPartitionPlan& viewPlan,
	    RayTracingPerformanceDiagnostics* diagnostics) noexcept = 0;

	virtual void Clear() noexcept = 0;

protected:
	RayTracingAccelerationStrategy() noexcept;
};

std::unique_ptr<RayTracingAccelerationStrategy> CreateRayTracingAccelerationStrategy(RenderHardwareInterface& renderHardwareInterface, const RayTracingCapabilityReport& capabilityReport) noexcept;
