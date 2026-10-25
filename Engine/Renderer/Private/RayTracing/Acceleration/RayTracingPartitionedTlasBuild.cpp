#include "PCH.h"

#include "RayTracing/Acceleration/RayTracingPartitionedTlasStrategy.h"

#include "Commands/RenderCommandContext.h"
#include "Meshes/GpuMesh.h"
#include "RayTracing/Acceleration/RayTracingBlasCache.h"
#include "RayTracing/Acceleration/RayTracingPtlasPartitionPlanner.h"
#include "RayTracing/Diagnostics/RayTracingPerformanceDiagnostics.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"
#include "RHI/Public/RayTracing/RhiRayTracingTransformPacking.h"
#include "Scene/Preparation/PreparedRenderScene.h"
#include "Scene/RayTracing/RayTracingShaderTablePlan.h"

#include <array>
#include <unordered_set>
#include <vector>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_rayTracingPartitionedTlasBuildLogger, "Renderer.RayTracing.PartitionedTlasBuild");

struct RayTracingPartitionedTlasStrategy::PartitionedBuildState final
{
	std::vector<RhiPartitionedTlasInstanceWriteDesc> InstanceWrites;
	std::unordered_set<void*> BuiltBlasResources;
	RhiPartitionedTlasOperationBufferLayout NativeOperationLayout = {};
};

RhiPartitionedTlasInstanceFlags RayTracingPartitionedTlasStrategy::ResolveInstanceFlags(const PreparedRenderScene& preparedScene, const MeshDraw& draw) noexcept
{
	RhiPartitionedTlasInstanceFlags flags = RhiPartitionedTlasInstanceFlags::None;
	if (draw.MaterialSlot >= preparedScene.materials.size())
	{
		Diagnostics::Fatal(g_rayTracingPartitionedTlasBuildLogger, __FILE__, __LINE__, "Partitioned TLAS input references a material outside the render scene.");
	}
	const MaterialData& material = preparedScene.materials[draw.MaterialSlot];
	if (material.doubleSided)
	{
		flags = flags | RhiPartitionedTlasInstanceFlags::TriangleFacingCullDisable;
	}
	if (material.alphaMode == 1u)
	{
		flags = flags | RhiPartitionedTlasInstanceFlags::ForceNoOpaque;
	}
	return flags;
}

RayTracingTopLevelAccelerationStructureBuildResult RayTracingPartitionedTlasStrategy::Build(
    RenderCommandContext& commandContext,
    const PreparedRenderScene& preparedScene,
    RayTracingBlasCache& blasCache,
    const RayTracingShaderTablePlan& shaderTablePlan,
    const RayTracingPtlasPartitionPlan& viewPlan,
    RayTracingPerformanceDiagnostics* diagnostics) noexcept
{
	RayTracingTopLevelAccelerationStructureBuildResult result{};
	result.ActiveProvider = ERhiRayTracingTopLevelProvider::PartitionedTlas;
	result.ActiveProviderReason = "partitioned-tlas-selected";
	const auto& instances = preparedScene.rayTracingInstances;

	if (!m_partitionedResources.Storage || !m_partitionedResources.Scratch)
	{
		Diagnostics::Fatal(g_rayTracingPartitionedTlasBuildLogger, __FILE__, __LINE__, "Partitioned TLAS build requires storage prepared before frame-graph binding.");
	}

	PartitionedBuildState state;
	state.InstanceWrites.reserve(instances.size());
	CollectPartitionedInstances(commandContext, preparedScene, viewPlan, blasCache, shaderTablePlan, diagnostics, state);
	const std::uint32_t nativeWriteCount = static_cast<std::uint32_t>(state.InstanceWrites.size());
	result.InstanceCount = nativeWriteCount;
	PreparePartitionedOperationBuffer(state);

	RecordPartitionedBuild(commandContext, state, diagnostics);
	return result;
}

void RayTracingPartitionedTlasStrategy::CollectPartitionedInstances(
    RenderCommandContext& commandContext,
    const PreparedRenderScene& preparedScene,
    const RayTracingPtlasPartitionPlan& partitionPlan,
    RayTracingBlasCache& blasCache,
    const RayTracingShaderTablePlan& shaderTablePlan,
    RayTracingPerformanceDiagnostics* diagnostics,
    PartitionedBuildState& state) noexcept
{
	const auto& instances = preparedScene.rayTracingInstances;
	for (const RenderRayTracingInstance& input : instances)
	{
		if (input.PrimitiveIndex >= preparedScene.primitives.size())
		{
			Diagnostics::Fatal(g_rayTracingPartitionedTlasBuildLogger, __FILE__, __LINE__, "Partitioned TLAS work references a mesh instance outside the render scene.");
		}
		const MeshDraw& draw = preparedScene.primitives[input.PrimitiveIndex].Draw;
		if (!draw.Geometry.Mesh)
		{
			Diagnostics::Fatal(g_rayTracingPartitionedTlasBuildLogger, __FILE__, __LINE__, "Partitioned TLAS work references a mesh instance with no GPU mesh handle.");
		}

		const RayTracingBlasCache::BlasHandle blas = blasCache.EnsureBlas(commandContext, preparedScene, draw, input.GpuSceneSlot, diagnostics);

		commandContext.TrackResource(blas.resource);
		if (blas.builtThisFrame)
		{
			state.BuiltBlasResources.insert(blas.resource.Value);
		}

		const RayTracingPtlasPartitionEntry* entry = partitionPlan.FindByPrimitive(input.PrimitiveIndex);
		if (entry == nullptr || !entry->Valid || entry->Identity.GpuSceneSlot != input.GpuSceneSlot)
		{
			Diagnostics::Fatal(g_rayTracingPartitionedTlasBuildLogger, __FILE__, __LINE__, "Partitioned TLAS work has no valid partition-plan entry.");
		}

		std::uint32_t instanceContribution = 0u;
		if (!shaderTablePlan.ResolveInstanceContribution(input.GpuSceneSlot, instanceContribution))
		{
			Diagnostics::Fatal(g_rayTracingPartitionedTlasBuildLogger, __FILE__, __LINE__, "Partitioned TLAS instance has no authoritative scene shader-table contribution.");
		}

		state.InstanceWrites.push_back(
		    RhiPartitionedTlasInstanceWriteDesc{
		        .Transform = RhiRayTracingTransformPacking::PackCanonicalObjectToWorld(draw.Transform.WorldMatrix),
		        .ExplicitBoundingBox = {},
		        .InstanceID = input.GpuSceneSlot,
		        .InstanceMask = 0xFFu,
		        .InstanceContributionToHitGroupIndex = instanceContribution,
		        .Flags = ResolveInstanceFlags(preparedScene, draw),
		        .InstanceIndex = input.GpuSceneSlot,
		        .PartitionIndex = entry->Update.UsesGlobalPartition ? RhiPartitionedTlasGlobalPartitionIndex : entry->Assignment.PartitionId,
		        .AccelerationStructure = blas.gpuAddress});
	}
}

void RayTracingPartitionedTlasStrategy::PreparePartitionedOperationBuffer(PartitionedBuildState& state) noexcept
{
	const std::uint32_t nativeWriteCount = static_cast<std::uint32_t>(state.InstanceWrites.size());
	const RhiPartitionedTlasOperationHeader operation{.Type = ERhiPartitionedTlasOperationType::WriteInstance, .ArgumentCount = nativeWriteCount, .ArgumentData = 0, .ArgumentStrideInBytes = 0};
	const RhiPartitionedTlasOperationPackDesc operationPack{.Operations = &operation, .OperationCount = 1, .InstanceWrites = state.InstanceWrites.data(), .InstanceWriteCount = nativeWriteCount};

	RhiRayTracingService& rayTracingService = m_renderHardwareInterface.GetRayTracingService();
	RhiResourceService& resourceService = m_renderHardwareInterface.GetResourceService();
	if (m_partitionedResources.NativeOperationData)
	{
		resourceService.ReleaseOwnedResource(m_partitionedResources.NativeOperationData);
		m_partitionedResources.NativeOperationData = {};
	}
	m_partitionedResources.NativeOperationData = rayTracingService.CreatePartitionedTopLevelAccelerationStructureOperationBuffer(operationPack, L"RayTracingPartitionedTlasCpuPackedOperations");
	if (!m_partitionedResources.NativeOperationData)
	{
		Diagnostics::Fatal(g_rayTracingPartitionedTlasBuildLogger, __FILE__, __LINE__, "Partitioned TLAS operation-buffer allocation failed.");
	}

	state.NativeOperationLayout = rayTracingService.GetPartitionedTopLevelAccelerationStructureOperationBufferLayout(m_partitionedResources.Layout);
	if (resourceService.GetResourceGpuVirtualAddress(m_partitionedResources.NativeOperationData) == 0)
	{
		Diagnostics::Fatal(g_rayTracingPartitionedTlasBuildLogger, __FILE__, __LINE__, "Partitioned TLAS operation buffer has no GPU address.");
	}
}

void RayTracingPartitionedTlasStrategy::RecordPartitionedBuild(RenderCommandContext& commandContext, const PartitionedBuildState& state, RayTracingPerformanceDiagnostics* diagnostics) const noexcept
{
	for (void* resourceValue : state.BuiltBlasResources)
	{
		commandContext.UnorderedAccessBarrier(RhiResourceHandle{resourceValue});
	}

	TrackBuildResources(commandContext);

	auto tlasGpuScope = diagnostics != nullptr ? diagnostics->BeginGpuScope("Partitioned TLAS Build") : ScopedGpuScope{};

	RhiResourceService& resources = m_renderHardwareInterface.GetResourceService();
	const RhiGpuVirtualAddress operationDataAddress = resources.GetResourceGpuVirtualAddress(m_partitionedResources.NativeOperationData);

	commandContext.BuildPartitionedTopLevelAccelerationStructure(
	    RhiPartitionedTlasBuildCommandDesc{
	        .Layout = m_partitionedResources.Layout,
	        .DestinationResource = resources.GetResourceHandle(m_partitionedResources.Storage),
	        .SourceAccelerationStructure = 0,
	        .DestinationAccelerationStructure = resources.GetResourceGpuVirtualAddress(m_partitionedResources.Storage),
	        .Scratch = resources.GetResourceGpuVirtualAddress(m_partitionedResources.Scratch),
	        .OperationHeaders = operationDataAddress + state.NativeOperationLayout.OperationHeadersOffsetInBytes,
	        .OperationCount = operationDataAddress + state.NativeOperationLayout.OperationCountOffsetInBytes});
}

void RayTracingPartitionedTlasStrategy::TrackBuildResources(RenderCommandContext& commandContext) const noexcept
{
	RhiResourceService& resources = m_renderHardwareInterface.GetResourceService();

	commandContext.TrackResource(resources.GetResourceHandle(m_partitionedResources.Storage));
	commandContext.TrackResource(resources.GetResourceHandle(m_partitionedResources.Scratch));
	commandContext.TrackResource(resources.GetResourceHandle(m_partitionedResources.NativeOperationData));
}
