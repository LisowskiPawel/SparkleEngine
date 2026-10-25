#include "PCH.h"

#include "RayTracing/Acceleration/RayTracingPtlasStrategy.h"

#include "Commands/RenderCommandContext.h"
#include "Meshes/GpuMesh.h"
#include "RayTracing/Acceleration/RayTracingBlasCache.h"
#include "RayTracing/Acceleration/RayTracingPtlasPartitionPlanner.h"
#include "RayTracing/Diagnostics/RayTracingPerformanceDiagnostics.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"
#include "RHI/Public/RayTracing/RhiRayTracingTransformPacking.h"
#include "Scene/Preparation/PreparedRenderScene.h"
#include "Scene/RayTracing/RayTracingShaderTablePlan.h"

#include <unordered_set>
#include <vector>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_rayTracingPtlasBuildLogger, "Renderer.RayTracing.PtlasBuild");

struct RayTracingPtlasStrategy::PtlasBuildState final
{
	std::vector<RhiPtlasInstanceWriteDesc> InstanceWrites;
	std::unordered_set<void*> BuiltBlasResources;
	RhiPtlasOperationBufferLayout NativeOperationLayout = {};
};

RhiPtlasInstanceFlags RayTracingPtlasStrategy::ResolveInstanceFlags(const PreparedRenderScene& preparedScene, const MeshDraw& draw) noexcept
{
	RhiPtlasInstanceFlags flags = RhiPtlasInstanceFlags::None;
	if (draw.MaterialSlot >= preparedScene.materials.size())
	{
		Diagnostics::Fatal(g_rayTracingPtlasBuildLogger, __FILE__, __LINE__, "PTLAS input references a material outside the render scene.");
	}
	const MaterialData& material = preparedScene.materials[draw.MaterialSlot];
	if (material.doubleSided)
	{
		flags = flags | RhiPtlasInstanceFlags::TriangleFacingCullDisable;
	}
	if (material.alphaMode == 1u)
	{
		flags = flags | RhiPtlasInstanceFlags::ForceNoOpaque;
	}
	return flags;
}

RayTracingTlasBuildResult RayTracingPtlasStrategy::Build(
    RenderCommandContext& commandContext,
    const PreparedRenderScene& preparedScene,
    RayTracingBlasCache& blasCache,
    const RayTracingShaderTablePlan& shaderTablePlan,
    const RayTracingPtlasPartitionPlan& viewPlan,
    RayTracingPerformanceDiagnostics* diagnostics) noexcept
{
	RayTracingTlasBuildResult result{};
	result.ActiveProvider = ERhiRayTracingTlasProvider::Ptlas;
	result.ActiveProviderReason = "PTLAS-selected";
	const auto& instances = preparedScene.rayTracingInstances;

	if (!m_ptlasResources.Storage || !m_ptlasResources.Scratch)
	{
		Diagnostics::Fatal(g_rayTracingPtlasBuildLogger, __FILE__, __LINE__, "PTLAS build requires storage prepared before frame-graph binding.");
	}

	PtlasBuildState state;
	state.InstanceWrites.reserve(instances.size());
	CollectPtlasInstances(commandContext, preparedScene, viewPlan, blasCache, shaderTablePlan, diagnostics, state);
	const std::uint32_t nativeWriteCount = static_cast<std::uint32_t>(state.InstanceWrites.size());
	result.InstanceCount = nativeWriteCount;
	PreparePtlasOperationBuffer(state);

	RecordPtlasBuild(commandContext, state, diagnostics);
	return result;
}

void RayTracingPtlasStrategy::CollectPtlasInstances(
    RenderCommandContext& commandContext,
    const PreparedRenderScene& preparedScene,
    const RayTracingPtlasPartitionPlan& partitionPlan,
    RayTracingBlasCache& blasCache,
    const RayTracingShaderTablePlan& shaderTablePlan,
    RayTracingPerformanceDiagnostics* diagnostics,
    PtlasBuildState& state) noexcept
{
	const auto& instances = preparedScene.rayTracingInstances;
	for (const RenderRayTracingInstance& input : instances)
	{
		if (input.PrimitiveIndex >= preparedScene.primitives.size())
		{
			Diagnostics::Fatal(g_rayTracingPtlasBuildLogger, __FILE__, __LINE__, "PTLAS work references a mesh instance outside the render scene.");
		}
		const MeshDraw& draw = preparedScene.primitives[input.PrimitiveIndex].Draw;
		if (!draw.Geometry.Mesh)
		{
			Diagnostics::Fatal(g_rayTracingPtlasBuildLogger, __FILE__, __LINE__, "PTLAS work references a mesh instance with no GPU mesh handle.");
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
			Diagnostics::Fatal(g_rayTracingPtlasBuildLogger, __FILE__, __LINE__, "PTLAS work has no valid partition-plan entry.");
		}

		std::uint32_t instanceContribution = 0u;
		if (!shaderTablePlan.ResolveInstanceContribution(input.GpuSceneSlot, instanceContribution))
		{
			Diagnostics::Fatal(g_rayTracingPtlasBuildLogger, __FILE__, __LINE__, "PTLAS instance has no authoritative scene shader-table contribution.");
		}

		state.InstanceWrites.push_back(
		    RhiPtlasInstanceWriteDesc{
		        .Transform = RhiRayTracingTransformPacking::PackCanonicalObjectToWorld(draw.Transform.WorldMatrix),
		        .ExplicitBoundingBox = {},
		        .InstanceID = input.GpuSceneSlot,
		        .InstanceMask = 0xFFu,
		        .InstanceContributionToHitGroupIndex = instanceContribution,
		        .Flags = ResolveInstanceFlags(preparedScene, draw),
		        .InstanceIndex = input.GpuSceneSlot,
		        .PartitionIndex = entry->Update.UsesGlobalPartition ? RhiPtlasGlobalPartitionIndex : entry->Assignment.PartitionId,
		        .AccelerationStructure = blas.gpuAddress});
	}
}

void RayTracingPtlasStrategy::PreparePtlasOperationBuffer(PtlasBuildState& state) noexcept
{
	const std::uint32_t nativeWriteCount = static_cast<std::uint32_t>(state.InstanceWrites.size());
	const RhiPtlasOperationHeader operation{.Type = ERhiPtlasOperationType::WriteInstance, .ArgumentCount = nativeWriteCount, .ArgumentData = 0, .ArgumentStrideInBytes = 0};
	const RhiPtlasOperationPackDesc operationPack{.Operations = &operation, .OperationCount = 1, .InstanceWrites = state.InstanceWrites.data(), .InstanceWriteCount = nativeWriteCount};

	RhiRayTracingService& rayTracingService = m_renderHardwareInterface.GetRayTracingService();
	RhiResourceService& resourceService = m_renderHardwareInterface.GetResourceService();
	if (m_ptlasResources.NativeOperationData)
	{
		resourceService.ReleaseOwnedResource(m_ptlasResources.NativeOperationData);
		m_ptlasResources.NativeOperationData = {};
	}
	m_ptlasResources.NativeOperationData = rayTracingService.CreatePtlasOperationBuffer(operationPack, L"RayTracingPtlasCpuPackedOperations");
	if (!m_ptlasResources.NativeOperationData)
	{
		Diagnostics::Fatal(g_rayTracingPtlasBuildLogger, __FILE__, __LINE__, "PTLAS operation-buffer allocation failed.");
	}

	state.NativeOperationLayout = rayTracingService.GetPtlasOperationBufferLayout(m_ptlasResources.Layout);
	if (resourceService.GetResourceGpuVirtualAddress(m_ptlasResources.NativeOperationData) == 0)
	{
		Diagnostics::Fatal(g_rayTracingPtlasBuildLogger, __FILE__, __LINE__, "PTLAS operation buffer has no GPU address.");
	}
}

void RayTracingPtlasStrategy::RecordPtlasBuild(RenderCommandContext& commandContext, const PtlasBuildState& state, RayTracingPerformanceDiagnostics* diagnostics) const noexcept
{
	for (void* resourceValue : state.BuiltBlasResources)
	{
		commandContext.UnorderedAccessBarrier(RhiResourceHandle{resourceValue});
	}

	TrackBuildResources(commandContext);

	auto tlasGpuScope = diagnostics != nullptr ? diagnostics->BeginGpuScope("PTLAS Build") : ScopedGpuScope{};

	RhiResourceService& resources = m_renderHardwareInterface.GetResourceService();
	const RhiGpuVirtualAddress operationDataAddress = resources.GetResourceGpuVirtualAddress(m_ptlasResources.NativeOperationData);

	commandContext.BuildPtlas(
	    RhiPtlasBuildCommandDesc{
	        .Layout = m_ptlasResources.Layout,
	        .DestinationResource = resources.GetResourceHandle(m_ptlasResources.Storage),
	        .SourceAccelerationStructure = 0,
	        .DestinationAccelerationStructure = resources.GetResourceGpuVirtualAddress(m_ptlasResources.Storage),
	        .Scratch = resources.GetResourceGpuVirtualAddress(m_ptlasResources.Scratch),
	        .OperationHeaders = operationDataAddress + state.NativeOperationLayout.OperationHeadersOffsetInBytes,
	        .OperationCount = operationDataAddress + state.NativeOperationLayout.OperationCountOffsetInBytes});
}

void RayTracingPtlasStrategy::TrackBuildResources(RenderCommandContext& commandContext) const noexcept
{
	RhiResourceService& resources = m_renderHardwareInterface.GetResourceService();

	commandContext.TrackResource(resources.GetResourceHandle(m_ptlasResources.Storage));
	commandContext.TrackResource(resources.GetResourceHandle(m_ptlasResources.Scratch));
	commandContext.TrackResource(resources.GetResourceHandle(m_ptlasResources.NativeOperationData));
}
