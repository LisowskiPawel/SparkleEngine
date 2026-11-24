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
	void Build(
	    RenderCommandList& commandList,
	    const PreparedRenderScene& preparedScene,
	    RayTracingBlasCache& blasCache,
	    const RayTracingShaderTablePlan& shaderTablePlan,
	    const RayTracingPtlasPartitionPlan& viewPlan,
	    PassExecutionDiagnostics* diagnostics) noexcept override;
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
	    RenderCommandList& commandList,
	    const PreparedRenderScene& preparedScene,
	    const RayTracingPtlasPartitionPlan& partitionPlan,
	    RayTracingBlasCache& blasCache,
	    const RayTracingShaderTablePlan& shaderTablePlan,
	    PassExecutionDiagnostics* diagnostics,
	    PtlasBuildState& state) noexcept;
	void PreparePtlasOperationBuffer(PtlasBuildState& state) noexcept;
	void RecordPtlasBuild(RenderCommandList& commandList, const PtlasBuildState& state, PassExecutionDiagnostics* diagnostics) const noexcept;
	void TrackBuildResources(RenderCommandList& commandList) const noexcept;
	void ReleasePtlasResources() noexcept;

	RenderHardwareInterface& m_renderHardwareInterface;
	PtlasResources m_ptlasResources;
};
