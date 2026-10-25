#include "PCH.h"

#include "RayTracing/Acceleration/RayTracingPtlasStrategy.h"

#include "RayTracing/RayTracingCapabilityReport.h"
#include "RayTracing/Acceleration/RayTracingPtlasPartitionPlanner.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"
#include "Scene/Preparation/PreparedRenderScene.h"

#include <algorithm>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_rayTracingPtlasStrategyLogger, "Renderer.RayTracing.PtlasStrategy");

RayTracingPtlasStrategy::RayTracingPtlasStrategy(RenderHardwareInterface& renderHardwareInterface, const RayTracingCapabilityReport& capabilityReport) noexcept :
    m_renderHardwareInterface(renderHardwareInterface)
{
	const RhiPtlasCapabilities& nativeCapabilities = renderHardwareInterface.GetCapabilities().RayTracing.Groups.Ptlas;
	if (!capabilityReport.Ptlas.Supported || !capabilityReport.Ptlas.SupportsDescriptor || !nativeCapabilities.SupportsCpuPackedOperations || nativeCapabilities.MaxOperationsPerBuild < 1u)
	{
		Diagnostics::Fatal(g_rayTracingPtlasStrategyLogger, __FILE__, __LINE__, "PTLAS requires descriptor access and CPU-packed instance writes.");
	}
}

RayTracingPtlasStrategy::~RayTracingPtlasStrategy() noexcept
{
	ReleasePtlasResources();
}

RenderRayTracingFrameBindings RayTracingPtlasStrategy::Prepare(const PreparedRenderScene& preparedScene, const RayTracingPtlasPartitionPlan& viewPlan) noexcept
{
	const RhiPtlasDesc layout = BuildPtlasLayout(preparedScene, viewPlan);
	EnsurePtlasResources(layout);
	return RenderRayTracingFrameBindings{.TlasResource = m_ptlasResources.Storage, .EstimatedInstanceCount = static_cast<std::uint32_t>(preparedScene.rayTracingInstances.size())};
}

void RayTracingPtlasStrategy::Clear() noexcept
{
	ReleasePtlasResources();
}

std::uint32_t RayTracingPtlasStrategy::ResolveInstanceCapacity(const PreparedRenderScene& preparedScene) noexcept
{
	std::uint32_t instanceCapacity = 0;
	for (const RenderRayTracingInstance& input : preparedScene.rayTracingInstances)
	{
		instanceCapacity = (std::max) (instanceCapacity, input.GpuSceneSlot + 1u);
	}
	return instanceCapacity;
}

RhiPtlasDesc RayTracingPtlasStrategy::BuildPtlasLayout(const PreparedRenderScene& preparedScene, const RayTracingPtlasPartitionPlan& partitionPlan) noexcept
{
	if (preparedScene.rayTracingInstances.empty())
	{
		return RhiPtlasDesc{.InstanceCapacity = 1, .PartitionCount = 1, .MaxInstancesPerPartition = 1, .MaxOperations = 1};
	}
	if (partitionPlan.Validation.HasDuplicateStableIndices || partitionPlan.Validation.HasPartitionOverflow || partitionPlan.Counts.GridPartitionCount == 0)
	{
		Diagnostics::Fatal(g_rayTracingPtlasStrategyLogger, __FILE__, __LINE__, "PTLAS requires a valid partition plan with unique GPU-scene slots.");
	}

	return RhiPtlasDesc{
	    .InstanceCapacity = ResolveInstanceCapacity(preparedScene),
	    .PartitionCount = partitionPlan.Counts.GridPartitionCount,
	    .MaxInstancesPerPartition = (std::max) (1u, partitionPlan.Counts.MaxInstancesPerPartition),
	    .MaxInstancesInGlobalPartition = partitionPlan.Counts.MaxInstancesInGlobalPartition,
	    .MaxOperations = 1};
}

void RayTracingPtlasStrategy::EnsurePtlasResources(const RhiPtlasDesc& requiredLayout) noexcept
{
	const RhiPtlasDesc& allocated = m_ptlasResources.Layout;

	const bool canReuse = m_ptlasResources.Storage && m_ptlasResources.Scratch && allocated.InstanceCapacity >= requiredLayout.InstanceCapacity
	    && allocated.PartitionCount == requiredLayout.PartitionCount && allocated.MaxInstancesPerPartition >= requiredLayout.MaxInstancesPerPartition
	    && allocated.MaxInstancesInGlobalPartition >= requiredLayout.MaxInstancesInGlobalPartition;

	if (canReuse)
	{
		return;
	}

	RhiRayTracingService& rayTracingService = m_renderHardwareInterface.GetRayTracingService();
	const RhiPtlasBuildSizes buildSizes = rayTracingService.GetPtlasBuildSizes(requiredLayout);
	if (buildSizes.AccelerationStructureSizeInBytes == 0 || buildSizes.BuildScratchSizeInBytes == 0)
	{
		Diagnostics::Fatal(g_rayTracingPtlasStrategyLogger, __FILE__, __LINE__, "PTLAS build sizing produced zero-sized storage.");
	}

	ReleasePtlasResources();
	m_ptlasResources.Storage = rayTracingService.CreatePtlasBuffer(buildSizes, L"RayTracingPtlas");
	m_ptlasResources.Scratch = rayTracingService.CreateRayTracingScratchBuffer(buildSizes.BuildScratchSizeInBytes, L"RayTracingPtlasScratch");
	m_ptlasResources.Layout = requiredLayout;
	RhiResourceService& resources = m_renderHardwareInterface.GetResourceService();
	if (!m_ptlasResources.Storage || !m_ptlasResources.Scratch || resources.GetResourceGpuVirtualAddress(m_ptlasResources.Storage) == 0
	    || resources.GetResourceGpuVirtualAddress(m_ptlasResources.Scratch) == 0)
	{
		Diagnostics::Fatal(g_rayTracingPtlasStrategyLogger, __FILE__, __LINE__, "PTLAS storage or scratch allocation has no GPU address.");
	}
}

void RayTracingPtlasStrategy::ReleasePtlasResources() noexcept
{
	RhiResourceService& resources = m_renderHardwareInterface.GetResourceService();
	if (m_ptlasResources.NativeOperationData)
	{
		resources.ReleaseOwnedResource(m_ptlasResources.NativeOperationData);
	}
	if (m_ptlasResources.Scratch)
	{
		resources.ReleaseOwnedResource(m_ptlasResources.Scratch);
	}
	if (m_ptlasResources.Storage)
	{
		resources.ReleaseOwnedResource(m_ptlasResources.Storage);
	}
	m_ptlasResources = {};
}
