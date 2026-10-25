#pragma once

#include "RayTracing/Acceleration/RayTracingTopLevelAccelerationStructureStrategy.h"
#include "RHI/Public/RayTracing/RhiPartitionedTlasDesc.h"

class RenderHardwareInterface;
struct MeshDraw;
struct RayTracingCapabilityReport;
struct RayTracingPtlasPartitionPlan;

class RayTracingPartitionedTlasStrategy final : public RayTracingTopLevelAccelerationStructureStrategy
{
public:
	RayTracingPartitionedTlasStrategy(RenderHardwareInterface& renderHardwareInterface, const RayTracingCapabilityReport& capabilityReport) noexcept;
	~RayTracingPartitionedTlasStrategy() noexcept override;

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
	struct PartitionedBuildState;

	struct PartitionedTlasResources final
	{
		RhiOwnedResourceHandle Storage = {};
		RhiOwnedResourceHandle Scratch = {};
		RhiOwnedResourceHandle NativeOperationData = {};
		RhiPartitionedTlasDesc Layout = {};
	};

	static std::uint32_t ResolveInstanceCapacity(const PreparedRenderScene& preparedScene) noexcept;
	static RhiPartitionedTlasDesc BuildPartitionedTlasLayout(const PreparedRenderScene& preparedScene, const RayTracingPtlasPartitionPlan& partitionPlan) noexcept;
	static RhiPartitionedTlasInstanceFlags ResolveInstanceFlags(const PreparedRenderScene& preparedScene, const MeshDraw& draw) noexcept;
	void EnsurePartitionedTlasResources(const RhiPartitionedTlasDesc& requiredLayout) noexcept;
	static void CollectPartitionedInstances(
	    RenderCommandContext& commandContext,
	    const PreparedRenderScene& preparedScene,
	    const RayTracingPtlasPartitionPlan& partitionPlan,
	    RayTracingBlasCache& blasCache,
	    const RayTracingShaderTablePlan& shaderTablePlan,
	    RayTracingPerformanceDiagnostics* diagnostics,
	    PartitionedBuildState& state) noexcept;
	void PreparePartitionedOperationBuffer(PartitionedBuildState& state) noexcept;
	void RecordPartitionedBuild(RenderCommandContext& commandContext, const PartitionedBuildState& state, RayTracingPerformanceDiagnostics* diagnostics) const noexcept;
	void TrackBuildResources(RenderCommandContext& commandContext) const noexcept;
	void ReleasePartitionedTlasResources() noexcept;

	RenderHardwareInterface& m_renderHardwareInterface;
	PartitionedTlasResources m_partitionedResources;
};
