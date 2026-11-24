#pragma once

#include "Scene/RayTracing/RenderRayTracingFrameBindings.h"
#include "RHI/Public/RayTracing/RhiRayTracingDesc.h"

#include <cstdint>
#include <memory>

class RayTracingBlasCache;
class PassExecutionDiagnostics;
class RayTracingShaderTablePlan;
class RenderCommandList;
class RenderHardwareInterface;
struct RayTracingCapabilityReport;
struct PreparedRenderScene;
struct RayTracingPtlasPartitionPlan;

class RayTracingAccelerationStrategy
{
public:
	virtual ~RayTracingAccelerationStrategy() noexcept;

	RayTracingAccelerationStrategy(const RayTracingAccelerationStrategy&) = delete;
	RayTracingAccelerationStrategy& operator=(const RayTracingAccelerationStrategy&) = delete;
	RayTracingAccelerationStrategy(RayTracingAccelerationStrategy&&) = delete;
	RayTracingAccelerationStrategy& operator=(RayTracingAccelerationStrategy&&) = delete;
	virtual RenderRayTracingFrameBindings Prepare(const PreparedRenderScene& preparedScene, const RayTracingPtlasPartitionPlan& viewPlan) noexcept = 0;

	virtual void Build(
	    RenderCommandList& commandList,
	    const PreparedRenderScene& preparedScene,
	    RayTracingBlasCache& blasCache,
	    const RayTracingShaderTablePlan& shaderTablePlan,
	    const RayTracingPtlasPartitionPlan& viewPlan,
	    PassExecutionDiagnostics* diagnostics) noexcept = 0;

	virtual void Clear() noexcept = 0;

protected:
	RayTracingAccelerationStrategy() noexcept;
};

std::unique_ptr<RayTracingAccelerationStrategy> CreateRayTracingAccelerationStrategy(RenderHardwareInterface& renderHardwareInterface, const RayTracingCapabilityReport& capabilityReport) noexcept;
