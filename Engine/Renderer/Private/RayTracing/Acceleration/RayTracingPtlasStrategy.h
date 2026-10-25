#pragma once

#include "RayTracing/Acceleration/RayTracingAccelerationStrategy.h"
#include "RHI/Public/RayTracing/RhiPtlasDesc.h"

class RenderHardwareInterface;
struct MeshDraw;
struct RayTracingCapabilityReport;
struct RayTracingPtlasPartitionPlan;

class RayTracingPtlasStrategy final : public RayTracingAccelerationStrategy
{
public:
	RayTracingPtlasStrategy(RenderHardwareInterface& renderHardwareInterface, const RayTracingCapabilityReport& capabilityReport) noexcept;
	~RayTracingPtlasStrategy() noexcept override;

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
	struct PtlasBuildState;

	struct PtlasResources final
	{
		RhiOwnedResourceHandle Storage = {};
		RhiOwnedResourceHandle Scratch = {};
		RhiOwnedResourceHandle NativeOperationData = {};
		RhiPtlasDesc Layout = {};
	};

	static std::uint32_t ResolveInstanceCapacity(const PreparedRenderScene& preparedScene) noexcept;
	static RhiPtlasDesc BuildPtlasLayout(const PreparedRenderScene& preparedScene, const RayTracingPtlasPartitionPlan& partitionPlan) noexcept;
	static RhiPtlasInstanceFlags ResolveInstanceFlags(const PreparedRenderScene& preparedScene, const MeshDraw& draw) noexcept;
	void EnsurePtlasResources(const RhiPtlasDesc& requiredLayout) noexcept;
	static void CollectPtlasInstances(
	    RenderCommandContext& commandContext,
	    const PreparedRenderScene& preparedScene,
	    const RayTracingPtlasPartitionPlan& partitionPlan,
	    RayTracingBlasCache& blasCache,
	    const RayTracingShaderTablePlan& shaderTablePlan,
	    RayTracingPerformanceDiagnostics* diagnostics,
	    PtlasBuildState& state) noexcept;
	void PreparePtlasOperationBuffer(PtlasBuildState& state) noexcept;
	void RecordPtlasBuild(RenderCommandContext& commandContext, const PtlasBuildState& state, RayTracingPerformanceDiagnostics* diagnostics) const noexcept;
	void TrackBuildResources(RenderCommandContext& commandContext) const noexcept;
	void ReleasePtlasResources() noexcept;

	RenderHardwareInterface& m_renderHardwareInterface;
	PtlasResources m_ptlasResources;
};
