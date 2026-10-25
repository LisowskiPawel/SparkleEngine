#include "PCH.h"

#include "RayTracing/Acceleration/RayTracingPartitionedTlasStrategy.h"

#include "RayTracing/RayTracingCapabilityReport.h"
#include "RayTracing/Acceleration/RayTracingPtlasPartitionPlanner.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"
#include "Scene/Preparation/PreparedRenderScene.h"

#include <algorithm>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_rayTracingPartitionedTlasStrategyLogger, "Renderer.RayTracing.PartitionedTlasStrategy");

RayTracingPartitionedTlasStrategy::RayTracingPartitionedTlasStrategy(RenderHardwareInterface& renderHardwareInterface, const RayTracingCapabilityReport& capabilityReport) noexcept :
    m_renderHardwareInterface(renderHardwareInterface)
{
	const RhiPartitionedTlasCapabilities& nativeCapabilities = renderHardwareInterface.GetCapabilities().RayTracing.Groups.PartitionedTlas;
	if (!capabilityReport.PartitionedTlas.Supported || !capabilityReport.PartitionedTlas.SupportsDescriptor || !nativeCapabilities.SupportsCpuPackedOperations
	    || nativeCapabilities.MaxOperationsPerBuild < 1u)
	{
		Diagnostics::Fatal(g_rayTracingPartitionedTlasStrategyLogger, __FILE__, __LINE__, "Partitioned TLAS requires descriptor access and CPU-packed instance writes.");
	}
}

RayTracingPartitionedTlasStrategy::~RayTracingPartitionedTlasStrategy() noexcept
{
	ReleasePartitionedTlasResources();
}

RenderRayTracingFrameBindings RayTracingPartitionedTlasStrategy::Prepare(const PreparedRenderScene& preparedScene, const RayTracingPtlasPartitionPlan& viewPlan) noexcept
{
	const RhiPartitionedTlasDesc layout = BuildPartitionedTlasLayout(preparedScene, viewPlan);
	EnsurePartitionedTlasResources(layout);
	return RenderRayTracingFrameBindings{.TlasResource = m_partitionedResources.Storage, .EstimatedInstanceCount = static_cast<std::uint32_t>(preparedScene.rayTracingInstances.size())};
}

void RayTracingPartitionedTlasStrategy::Clear() noexcept
{
	ReleasePartitionedTlasResources();
}

std::uint32_t RayTracingPartitionedTlasStrategy::ResolveInstanceCapacity(const PreparedRenderScene& preparedScene) noexcept
{
	std::uint32_t instanceCapacity = 0;
	for (const RenderRayTracingInstance& input : preparedScene.rayTracingInstances)
	{
		instanceCapacity = (std::max) (instanceCapacity, input.GpuSceneSlot + 1u);
	}
	return instanceCapacity;
}

RhiPartitionedTlasDesc RayTracingPartitionedTlasStrategy::BuildPartitionedTlasLayout(const PreparedRenderScene& preparedScene, const RayTracingPtlasPartitionPlan& partitionPlan) noexcept
{
	if (preparedScene.rayTracingInstances.empty())
	{
		return RhiPartitionedTlasDesc{.InstanceCapacity = 1, .PartitionCount = 1, .MaxInstancesPerPartition = 1, .MaxOperations = 1};
	}
	if (partitionPlan.Validation.HasDuplicateStableIndices || partitionPlan.Validation.HasPartitionOverflow || partitionPlan.Counts.GridPartitionCount == 0)
	{
		Diagnostics::Fatal(g_rayTracingPartitionedTlasStrategyLogger, __FILE__, __LINE__, "Partitioned TLAS requires a valid partition plan with unique GPU-scene slots.");
	}

	return RhiPartitionedTlasDesc{
	    .InstanceCapacity = ResolveInstanceCapacity(preparedScene),
	    .PartitionCount = partitionPlan.Counts.GridPartitionCount,
	    .MaxInstancesPerPartition = (std::max) (1u, partitionPlan.Counts.MaxInstancesPerPartition),
	    .MaxInstancesInGlobalPartition = partitionPlan.Counts.MaxInstancesInGlobalPartition,
	    .MaxOperations = 1};
}

void RayTracingPartitionedTlasStrategy::EnsurePartitionedTlasResources(const RhiPartitionedTlasDesc& requiredLayout) noexcept
{
	const RhiPartitionedTlasDesc& allocated = m_partitionedResources.Layout;

	const bool canReuse = m_partitionedResources.Storage && m_partitionedResources.Scratch && allocated.InstanceCapacity >= requiredLayout.InstanceCapacity
	    && allocated.PartitionCount == requiredLayout.PartitionCount && allocated.MaxInstancesPerPartition >= requiredLayout.MaxInstancesPerPartition
	    && allocated.MaxInstancesInGlobalPartition >= requiredLayout.MaxInstancesInGlobalPartition;

	if (canReuse)
	{
		return;
	}

	RhiRayTracingService& rayTracingService = m_renderHardwareInterface.GetRayTracingService();
	const RhiPartitionedTlasBuildSizes buildSizes = rayTracingService.GetPartitionedTopLevelAccelerationStructureBuildSizes(requiredLayout);
	if (buildSizes.AccelerationStructureSizeInBytes == 0 || buildSizes.BuildScratchSizeInBytes == 0)
	{
		Diagnostics::Fatal(g_rayTracingPartitionedTlasStrategyLogger, __FILE__, __LINE__, "Partitioned TLAS build sizing produced zero-sized storage.");
	}

	ReleasePartitionedTlasResources();
	m_partitionedResources.Storage = rayTracingService.CreatePartitionedTopLevelAccelerationStructureBuffer(buildSizes, L"RayTracingPartitionedTlas");
	m_partitionedResources.Scratch = rayTracingService.CreateRayTracingScratchBuffer(buildSizes.BuildScratchSizeInBytes, L"RayTracingPartitionedTlasScratch");
	m_partitionedResources.Layout = requiredLayout;
	RhiResourceService& resources = m_renderHardwareInterface.GetResourceService();
	if (!m_partitionedResources.Storage || !m_partitionedResources.Scratch || resources.GetResourceGpuVirtualAddress(m_partitionedResources.Storage) == 0
	    || resources.GetResourceGpuVirtualAddress(m_partitionedResources.Scratch) == 0)
	{
		Diagnostics::Fatal(g_rayTracingPartitionedTlasStrategyLogger, __FILE__, __LINE__, "Partitioned TLAS storage or scratch allocation has no GPU address.");
	}
}

void RayTracingPartitionedTlasStrategy::ReleasePartitionedTlasResources() noexcept
{
	RhiResourceService& resources = m_renderHardwareInterface.GetResourceService();
	if (m_partitionedResources.NativeOperationData)
	{
		resources.ReleaseOwnedResource(m_partitionedResources.NativeOperationData);
	}
	if (m_partitionedResources.Scratch)
	{
		resources.ReleaseOwnedResource(m_partitionedResources.Scratch);
	}
	if (m_partitionedResources.Storage)
	{
		resources.ReleaseOwnedResource(m_partitionedResources.Storage);
	}
	m_partitionedResources = {};
}
