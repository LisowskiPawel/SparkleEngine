#include "PCH.h"
#include "RHI/Public/RayTracing/RhiRayTracingService.h"
#include "RHI/Public/RayTracing/RhiPtlasService.h"
#include "RHI/Public/Resources/RhiResourceService.h"

#include "RayTracing/Acceleration/RayTracingPtlasStrategy.h"

#include "RHI/Public/Commands/RenderCommandList.h"
#include "Meshes/GpuMesh.h"
#include "RayTracing/Acceleration/RayTracingBlasCache.h"
#include "RayTracing/Acceleration/RayTracingPtlasPartitionPlanner.h"
#include "Diagnostics/PassExecutionDiagnostics.h"
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
	if (material.bDoubleSided)
	{
		flags = flags | RhiPtlasInstanceFlags::TriangleFacingCullDisable;
	}
	if (material.alphaMode == 1u)
	{
		flags = flags | RhiPtlasInstanceFlags::ForceNoOpaque;
	}
	return flags;
}

void RayTracingPtlasStrategy::Build(
    RenderCommandList& commandList,
    const PreparedRenderScene& preparedScene,
    RayTracingBlasCache& blasCache,
    const RayTracingShaderTablePlan& shaderTablePlan,
    const RayTracingPtlasPartitionPlan& viewPlan,
    PassExecutionDiagnostics* diagnostics) noexcept
{
	const auto& instances = preparedScene.rayTracingInstances;

	if (!m_ptlasResources.Storage || !m_ptlasResources.Scratch)
	{
		Diagnostics::Fatal(g_rayTracingPtlasBuildLogger, __FILE__, __LINE__, "PTLAS build requires storage prepared before frame-graph binding.");
	}

	PtlasBuildState state;
	state.InstanceWrites.reserve(instances.size());
	CollectPtlasInstances(commandList, preparedScene, viewPlan, blasCache, shaderTablePlan, diagnostics, state);
	PreparePtlasOperationBuffer(state);

	RecordPtlasBuild(commandList, state, diagnostics);
}

void RayTracingPtlasStrategy::CollectPtlasInstances(
    RenderCommandList& commandList,
    const PreparedRenderScene& preparedScene,
    const RayTracingPtlasPartitionPlan& partitionPlan,
    RayTracingBlasCache& blasCache,
    const RayTracingShaderTablePlan& shaderTablePlan,
    PassExecutionDiagnostics* diagnostics,
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

		const RayTracingBlasCache::BlasHandle blas = blasCache.EnsureBlas(commandList, preparedScene, draw, input.GpuSceneSlot, diagnostics);

		commandList.TrackResource(blas.resource);
		if (blas.bBuiltThisFrame)
		{
			state.BuiltBlasResources.insert(blas.resource.Value);
		}

		const RayTracingPtlasPartitionEntry* entry = partitionPlan.FindByPrimitive(input.PrimitiveIndex);
		if (entry == nullptr || !entry->bValid || entry->Identity.GpuSceneSlot != input.GpuSceneSlot)
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
		        .PartitionIndex = entry->Update.bUsesGlobalPartition ? RhiPtlasGlobalPartitionIndex : entry->Assignment.PartitionId,
		        .AccelerationStructure = blas.gpuAddress});
	}
}

void RayTracingPtlasStrategy::PreparePtlasOperationBuffer(PtlasBuildState& state) noexcept
{
	const std::uint32_t nativeWriteCount = static_cast<std::uint32_t>(state.InstanceWrites.size());
	const RhiPtlasOperationHeader operation{.Type = ERhiPtlasOperationType::WriteInstance, .ArgumentCount = nativeWriteCount, .ArgumentData = 0, .ArgumentStrideInBytes = 0};
	const RhiPtlasOperationPackDesc operationPack{.Operations = &operation, .OperationCount = 1, .InstanceWrites = state.InstanceWrites.data(), .InstanceWriteCount = nativeWriteCount};

	RhiPtlasService& ptlasService = m_renderHardwareInterface.GetRayTracingService().GetPtlasService();
	RhiResourceService& resourceService = m_renderHardwareInterface.GetResourceService();
	if (m_ptlasResources.NativeOperationData)
	{
		resourceService.ReleaseOwnedResource(m_ptlasResources.NativeOperationData);
		m_ptlasResources.NativeOperationData = {};
	}
	m_ptlasResources.NativeOperationData = ptlasService.CreatePtlasOperationBuffer(operationPack, L"RayTracingPtlasCpuPackedOperations");
	if (!m_ptlasResources.NativeOperationData)
	{
		Diagnostics::Fatal(g_rayTracingPtlasBuildLogger, __FILE__, __LINE__, "PTLAS operation-buffer allocation failed.");
	}

	state.NativeOperationLayout = ptlasService.GetPtlasOperationBufferLayout(m_ptlasResources.Layout);
	if (resourceService.GetResourceGpuVirtualAddress(m_ptlasResources.NativeOperationData) == 0)
	{
		Diagnostics::Fatal(g_rayTracingPtlasBuildLogger, __FILE__, __LINE__, "PTLAS operation buffer has no GPU address.");
	}
}

void RayTracingPtlasStrategy::RecordPtlasBuild(RenderCommandList& commandList, const PtlasBuildState& state, PassExecutionDiagnostics* diagnostics) const noexcept
{
	for (void* resourceValue : state.BuiltBlasResources)
	{
		commandList.UnorderedAccessBarrier(RhiResourceHandle{resourceValue});
	}

	TrackBuildResources(commandList);

	auto tlasGpuScope = diagnostics != nullptr ? diagnostics->BeginGpuScope("PTLAS Build") : ScopedGpuScope{};

	RhiResourceService& resources = m_renderHardwareInterface.GetResourceService();
	const RhiGpuVirtualAddress operationDataAddress = resources.GetResourceGpuVirtualAddress(m_ptlasResources.NativeOperationData);

	commandList.BuildPtlas(
	    RhiPtlasBuildCommandDesc{
	        .Layout = m_ptlasResources.Layout,
	        .DestinationResource = resources.GetResourceHandle(m_ptlasResources.Storage),
	        .SourceAccelerationStructure = 0,
	        .DestinationAccelerationStructure = resources.GetResourceGpuVirtualAddress(m_ptlasResources.Storage),
	        .Scratch = resources.GetResourceGpuVirtualAddress(m_ptlasResources.Scratch),
	        .OperationHeaders = operationDataAddress + state.NativeOperationLayout.OperationHeadersOffsetInBytes,
	        .OperationCount = operationDataAddress + state.NativeOperationLayout.OperationCountOffsetInBytes});
}

void RayTracingPtlasStrategy::TrackBuildResources(RenderCommandList& commandList) const noexcept
{
	RhiResourceService& resources = m_renderHardwareInterface.GetResourceService();

	commandList.TrackResource(resources.GetResourceHandle(m_ptlasResources.Storage));
	commandList.TrackResource(resources.GetResourceHandle(m_ptlasResources.Scratch));
	commandList.TrackResource(resources.GetResourceHandle(m_ptlasResources.NativeOperationData));
}
