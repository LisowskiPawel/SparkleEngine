#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Commands/VulkanRenderCommandList.h"
#include "Vulkan/Pipeline/VulkanRayTracingPipeline.h"
#include "Vulkan/RayTracing/VulkanRayTracingShaderTable.h"

#include "Vulkan/Device/VulkanRhi.h"
#include "Vulkan/VulkanTypeConversions.h"
#include "Core/Public/Diagnostics/Verify.h"
#include "Validation/RhiContract.h"

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_vulkanRenderCommandListLogger, "RHI.Vulkan.CommandList");

void VulkanRenderCommandList::SetRayTracingPipeline(const RayTracingPipeline& pipeline) noexcept
{
	const auto* nativePipeline = dynamic_cast<const VulkanRayTracingPipeline*>(&pipeline);
	if (m_commandBuffer == VK_NULL_HANDLE || nativePipeline == nullptr)
	{
		Diagnostics::Fatal(g_vulkanRenderCommandListLogger, __FILE__, __LINE__, "Vulkan ray-tracing pipeline binding received no command buffer or a foreign pipeline.");
	}
	m_rayTracingBindings.PipelineLayout = nativePipeline->GetPipelineLayout();
	vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, nativePipeline->GetPipeline());
	m_boundRayTracingPipeline = &pipeline;
}

void VulkanRenderCommandList::BuildBottomLevelAccelerationStructure(const RhiRayTracingGeometryDesc& geometry, RhiGpuVirtualAddress scratchGpuAddress, RhiGpuVirtualAddress resultGpuAddress) noexcept
{
	const VkDeviceAddress vertexBufferAddress = ResolveRayTracingBufferAddress(geometry.VertexBuffer);
	const VkDeviceAddress indexBufferAddress = ResolveRayTracingBufferAddress(geometry.IndexBuffer);
	const bool bInvalidBuildInputsrhi = m_commandBuffer == VK_NULL_HANDLE || m_rhi == nullptr;
	const bool bInvalidBuildInputsmemoryAllocator = bInvalidBuildInputsrhi || (m_memoryAllocator == nullptr);
	const bool bBuildInputsAccelerationStructures = bInvalidBuildInputsmemoryAllocator || (m_rhi->GetCmdBuildAccelerationStructures() == nullptr);
	const bool bInvalidBuildInputsRhiContract = bBuildInputsAccelerationStructures || (!RhiContract::IsRayTracingGeometryDescUsable(geometry));
	const bool bInvalidBuildInputsBufferAddress = bInvalidBuildInputsRhiContract || (vertexBufferAddress == 0);
	const bool bInvalidBuildInputsBufferAddress2 = bInvalidBuildInputsBufferAddress || (indexBufferAddress == 0);
	const bool bInvalidBuildInputsGpuAddress = bInvalidBuildInputsBufferAddress2 || (scratchGpuAddress == 0);
	const bool bInvalidBlasBuildInputs = bInvalidBuildInputsGpuAddress || (resultGpuAddress == 0);
	
	if (bInvalidBlasBuildInputs)
	{
		Diagnostics::Fatal(g_vulkanRenderCommandListLogger, __FILE__, __LINE__, "Vulkan BLAS build received incomplete geometry, device, command-buffer, or GPU-address inputs.");
	}
	EndDynamicRenderingIfNeeded();

	TrackResource(geometry.VertexBuffer.Resource);
	TrackResource(geometry.IndexBuffer.Resource);

	VulkanRecordingResource resultResource;
	const bool bResultResolved = ResolveAddress(resultGpuAddress, resultResource);
	const bool bStructureMatchesStructure = bResultResolved && resultResource.AccelerationStructure != VK_NULL_HANDLE;
	const bool bAccelerationStructureMatches = bStructureMatchesStructure && (resultResource.AccelerationStructureType == VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR);
	
	if (!bAccelerationStructureMatches)
	{
		Diagnostics::Fatal(g_vulkanRenderCommandListLogger, __FILE__, __LINE__, "Vulkan BLAS destination does not resolve to a bottom-level acceleration structure.");
	}

	const VkAccelerationStructureGeometryTrianglesDataKHR triangles{
	    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR,
	    .pNext = nullptr,
	    .vertexFormat = VK_FORMAT_R32G32B32_SFLOAT,
	    .vertexData = VkDeviceOrHostAddressConstKHR{.deviceAddress = vertexBufferAddress},
	    .vertexStride = geometry.VertexStrideInBytes,
	    .maxVertex = geometry.VertexCount > 0 ? geometry.VertexCount - 1u : 0u,
	    .indexType = VulkanTypeConversions::ToVkIndexType(geometry.IndexFormat),
	    .indexData = VkDeviceOrHostAddressConstKHR{.deviceAddress = indexBufferAddress},
	    .transformData = VkDeviceOrHostAddressConstKHR{.deviceAddress = 0}};

	const VkAccelerationStructureGeometryKHR nativeGeometry{
	    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR,
	    .pNext = nullptr,
	    .geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR,
	    .geometry = VkAccelerationStructureGeometryDataKHR{.triangles = triangles},
	    .flags = geometry.bOpaque ? VK_GEOMETRY_OPAQUE_BIT_KHR : 0u};

	const VkAccelerationStructureBuildGeometryInfoKHR buildInfo{
	    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR,
	    .pNext = nullptr,
	    .type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
	    .flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR,
	    .mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR,
	    .srcAccelerationStructure = VK_NULL_HANDLE,
	    .dstAccelerationStructure = resultResource.AccelerationStructure,
	    .geometryCount = 1,
	    .pGeometries = &nativeGeometry,
	    .ppGeometries = nullptr,
	    .scratchData = VkDeviceOrHostAddressKHR{.deviceAddress = scratchGpuAddress}};

	const std::uint32_t primitiveCount = geometry.IndexCount / 3u;
	const VkAccelerationStructureBuildRangeInfoKHR rangeInfo{.primitiveCount = primitiveCount, .primitiveOffset = 0, .firstVertex = 0, .transformOffset = 0};
	const VkAccelerationStructureBuildRangeInfoKHR* rangeInfos[] = {&rangeInfo};
	m_rhi->GetCmdBuildAccelerationStructures()(m_commandBuffer, 1, &buildInfo, rangeInfos);

	const VkMemoryBarrier2 buildBarrier{
	    .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
	    .pNext = nullptr,
	    .srcStageMask = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
	    .srcAccessMask = VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR,
	    .dstStageMask = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
	    .dstAccessMask = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR};

	const VkDependencyInfo dependencyInfo{
	    .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
	    .pNext = nullptr,
	    .dependencyFlags = 0,
	    .memoryBarrierCount = 1,
	    .pMemoryBarriers = &buildBarrier,
	    .bufferMemoryBarrierCount = 0,
	    .pBufferMemoryBarriers = nullptr,
	    .imageMemoryBarrierCount = 0,
	    .pImageMemoryBarriers = nullptr};

	vkCmdPipelineBarrier2(m_commandBuffer, &dependencyInfo);
}

void VulkanRenderCommandList::BuildTlas(
    RhiGpuVirtualAddress instanceDescsGpuAddress,
    std::uint32_t instanceCount,
    RhiGpuVirtualAddress scratchGpuAddress,
    RhiGpuVirtualAddress resultGpuAddress,
    ERhiTlasBuildMode buildMode) noexcept
{
	const bool bInvalidBuildInputsrhi = m_commandBuffer == VK_NULL_HANDLE || m_rhi == nullptr;
	const bool bInvalidBuildInputsmemoryAllocator = bInvalidBuildInputsrhi || (m_memoryAllocator == nullptr);
	const bool bBuildInputsAccelerationStructures = bInvalidBuildInputsmemoryAllocator || (m_rhi->GetCmdBuildAccelerationStructures() == nullptr);
	const bool bInvalidBuildInputsGpuAddress = bBuildInputsAccelerationStructures || (instanceDescsGpuAddress == 0);
	const bool bInvalidBuildInputsGpuAddress2 = bInvalidBuildInputsGpuAddress || (scratchGpuAddress == 0);
	const bool bInvalidTlasBuildInputs = bInvalidBuildInputsGpuAddress2 || (resultGpuAddress == 0);
	
	if (bInvalidTlasBuildInputs)
	{
		Diagnostics::Fatal(g_vulkanRenderCommandListLogger, __FILE__, __LINE__, "Vulkan TLAS build received no device, command buffer, build entry point, or GPU address.");
	}
	EndDynamicRenderingIfNeeded();

	VulkanRecordingResource resultResource;
	const bool bResultResolved = ResolveAddress(resultGpuAddress, resultResource);
	const bool bStructureMatchesStructure = bResultResolved && resultResource.AccelerationStructure != VK_NULL_HANDLE;
	const bool bAccelerationStructureMatches = bStructureMatchesStructure && (resultResource.AccelerationStructureType == VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR);
	
	if (!bAccelerationStructureMatches)
	{
		Diagnostics::Fatal(g_vulkanRenderCommandListLogger, __FILE__, __LINE__, "Vulkan TLAS destination does not resolve to a TLAS.");
	}

	const VkAccelerationStructureGeometryInstancesDataKHR instances{
	    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR,
	    .pNext = nullptr,
	    .arrayOfPointers = VK_FALSE,
	    .data = VkDeviceOrHostAddressConstKHR{.deviceAddress = instanceDescsGpuAddress}};

	const VkAccelerationStructureGeometryKHR geometry{
	    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR,
	    .pNext = nullptr,
	    .geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR,
	    .geometry = VkAccelerationStructureGeometryDataKHR{.instances = instances},
	    .flags = VK_GEOMETRY_OPAQUE_BIT_KHR};

	const VkBuildAccelerationStructureFlagsKHR nativeBuildFlags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR
	    | (buildMode != ERhiTlasBuildMode::Build ? VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR : static_cast<VkBuildAccelerationStructureFlagsKHR>(0));

	const VkAccelerationStructureBuildGeometryInfoKHR buildInfo{
	    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR,
	    .pNext = nullptr,
	    .type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR,
	    .flags = nativeBuildFlags,
	    .mode = buildMode == ERhiTlasBuildMode::Update ? VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR : VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR,
	    .srcAccelerationStructure = buildMode == ERhiTlasBuildMode::Update ? resultResource.AccelerationStructure : VK_NULL_HANDLE,
	    .dstAccelerationStructure = resultResource.AccelerationStructure,
	    .geometryCount = 1,
	    .pGeometries = &geometry,
	    .ppGeometries = nullptr,

	    .scratchData = VkDeviceOrHostAddressKHR{.deviceAddress = scratchGpuAddress}};

	const VkAccelerationStructureBuildRangeInfoKHR rangeInfo{.primitiveCount = instanceCount, .primitiveOffset = 0, .firstVertex = 0, .transformOffset = 0};
	const VkAccelerationStructureBuildRangeInfoKHR* rangeInfos[] = {&rangeInfo};
	m_rhi->GetCmdBuildAccelerationStructures()(m_commandBuffer, 1, &buildInfo, rangeInfos);

	const VkMemoryBarrier2 buildBarrier{
	    .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
	    .pNext = nullptr,
	    .srcStageMask = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
	    .srcAccessMask = VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR,
	    .dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
	    .dstAccessMask = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR};

	const VkDependencyInfo dependencyInfo{
	    .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
	    .pNext = nullptr,
	    .dependencyFlags = 0,
	    .memoryBarrierCount = 1,
	    .pMemoryBarriers = &buildBarrier,
	    .bufferMemoryBarrierCount = 0,
	    .pBufferMemoryBarriers = nullptr,
	    .imageMemoryBarrierCount = 0,
	    .pImageMemoryBarriers = nullptr};

	vkCmdPipelineBarrier2(m_commandBuffer, &dependencyInfo);
}

void VulkanRenderCommandList::BuildPtlas(const RhiPtlasBuildCommandDesc& desc) noexcept
{
	const bool bInvalidBuildInputsrhi = m_commandBuffer == VK_NULL_HANDLE || m_rhi == nullptr;
	const bool bInvalidBuildInputsBuildPtlas = bInvalidBuildInputsrhi || (m_rhi->GetCmdBuildPtlas() == nullptr);
	const bool bBuildInputsDestinationResource = bInvalidBuildInputsBuildPtlas || (!desc.DestinationResource);
	const bool bBuildInputsAccelerationStructure = bBuildInputsDestinationResource || (desc.DestinationAccelerationStructure == 0);
	const bool bInvalidBuildInputsScratch = bBuildInputsAccelerationStructure || (desc.Scratch == 0);
	const bool bBuildInputsOperationHeaders = bInvalidBuildInputsScratch || (desc.OperationHeaders == 0);
	const bool bInvalidBuildInputsOperationCount = bBuildInputsOperationHeaders || (desc.OperationCount == 0);
	const bool bInvalidBuildInputsLayout = bInvalidBuildInputsOperationCount || (desc.Layout.InstanceCapacity == 0);
	const bool bInvalidPtlasBuildInputs = bInvalidBuildInputsLayout || (desc.Layout.PartitionCount == 0);
	
	if (bInvalidPtlasBuildInputs)
	{
		Diagnostics::Fatal(g_vulkanRenderCommandListLogger, __FILE__, __LINE__, "Vulkan PTLAS build received incomplete device, command-buffer, layout, or GPU-address inputs.");
	}
	EndDynamicRenderingIfNeeded();

	const VkMemoryBarrier2 operationDataBarrier{
	    .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
	    .pNext = nullptr,
	    .srcStageMask = VK_PIPELINE_STAGE_2_HOST_BIT | VK_PIPELINE_STAGE_2_TRANSFER_BIT | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
	    .srcAccessMask = VK_ACCESS_2_HOST_WRITE_BIT | VK_ACCESS_2_TRANSFER_WRITE_BIT | VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,
	    .dstStageMask = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
	    .dstAccessMask = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR};

	const VkDependencyInfo operationDataDependency{
	    .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
	    .pNext = nullptr,
	    .dependencyFlags = 0,
	    .memoryBarrierCount = 1,
	    .pMemoryBarriers = &operationDataBarrier,
	    .bufferMemoryBarrierCount = 0,
	    .pBufferMemoryBarriers = nullptr,
	    .imageMemoryBarrierCount = 0,
	    .pImageMemoryBarriers = nullptr};

	vkCmdPipelineBarrier2(m_commandBuffer, &operationDataDependency);

	VkPartitionedAccelerationStructureFlagsNV ptlasFlags{};
	VkPartitionedAccelerationStructureInstancesInputNV input{};
	VulkanTypeConversions::ConfigurePtlasInput(desc.Layout, input, ptlasFlags);

	const VkBuildPartitionedAccelerationStructureInfoNV buildInfo{
	    .sType = VK_STRUCTURE_TYPE_BUILD_PARTITIONED_ACCELERATION_STRUCTURE_INFO_NV,
	    .pNext = nullptr,
	    .input = input,
	    .srcAccelerationStructureData = desc.SourceAccelerationStructure,
	    .dstAccelerationStructureData = desc.DestinationAccelerationStructure,
	    .scratchData = desc.Scratch,
	    .srcInfos = desc.OperationHeaders,
	    .srcInfosCount = desc.OperationCount};

	m_rhi->GetCmdBuildPtlas()(m_commandBuffer, &buildInfo);

	const VkMemoryBarrier2 buildBarrier{
	    .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
	    .pNext = nullptr,
	    .srcStageMask = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
	    .srcAccessMask = VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR,
	    .dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
	    .dstAccessMask = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_2_SHADER_SAMPLED_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_READ_BIT};

	const VkDependencyInfo dependencyInfo{
	    .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
	    .pNext = nullptr,
	    .dependencyFlags = 0,
	    .memoryBarrierCount = 1,
	    .pMemoryBarriers = &buildBarrier,
	    .bufferMemoryBarrierCount = 0,
	    .pBufferMemoryBarriers = nullptr,
	    .imageMemoryBarrierCount = 0,
	    .pImageMemoryBarriers = nullptr};

	vkCmdPipelineBarrier2(m_commandBuffer, &dependencyInfo);
}

void VulkanRenderCommandList::TraceRays(const TraceRaysDesc& desc) noexcept
{
	try
	{
		RhiContract::ValidateTraceRaysDesc(desc, m_queueType);
	}
	catch (const std::exception& error)
	{
		Diagnostics::Fatal(g_vulkanRenderCommandListLogger, __FILE__, __LINE__, error.what());
	}
	const auto* pipeline = dynamic_cast<const VulkanRayTracingPipeline*>(desc.Pipeline);
	const auto* table = dynamic_cast<const VulkanRayTracingShaderTable*>(desc.ShaderTable);
	const bool bRayDispatchUnavailablerhi = m_commandBuffer == VK_NULL_HANDLE || m_rhi == nullptr;
	const bool bRayDispatchUnavailableTraceRays = bRayDispatchUnavailablerhi || (m_rhi->GetCmdTraceRays() == nullptr);
	const bool bRayDispatchUnavailablepipeline = bRayDispatchUnavailableTraceRays || (pipeline == nullptr);
	const bool bRayDispatchUnavailable = bRayDispatchUnavailablepipeline || (table == nullptr);
	
	if (bRayDispatchUnavailable)
	{
		Diagnostics::Fatal(g_vulkanRenderCommandListLogger, __FILE__, __LINE__, "Vulkan TraceRays requires a command buffer, loaded function, and matching native objects.");
	}
	if (m_boundRayTracingPipeline != desc.Pipeline)
	{
		Diagnostics::Fatal(g_vulkanRenderCommandListLogger, __FILE__, __LINE__, "Vulkan TraceRays requires its exact pipeline to be bound first.");
	}
	TrackResource(table->GetResource());
	const VkDeviceAddress baseAddress = table->GetDeviceAddress();

	const auto nativeRegion = [baseAddress](const RhiRayTracingShaderTableRegion& region)
	{
		return VkStridedDeviceAddressRegionKHR{.deviceAddress = region.SizeInBytes != 0 ? baseAddress + region.OffsetInBytes : 0, .stride = region.StrideInBytes, .size = region.SizeInBytes};
	};

	const VkStridedDeviceAddressRegionKHR rayGeneration = nativeRegion(desc.RayGeneration);
	const VkStridedDeviceAddressRegionKHR miss = nativeRegion(desc.Miss);
	const VkStridedDeviceAddressRegionKHR hitGroup = nativeRegion(desc.HitGroup);
	const VkStridedDeviceAddressRegionKHR callable = nativeRegion(desc.Callable);
	FlushRayTracingDescriptorSets();
	m_rhi->GetCmdTraceRays()(m_commandBuffer, &rayGeneration, &miss, &hitGroup, &callable, desc.Width, desc.Height, desc.Depth);
}

VkDeviceAddress VulkanRenderCommandList::ResolveRayTracingBufferAddress(const RhiRayTracingBufferBinding& binding) const noexcept
{
	VulkanRecordingResource resource;
	if (!ResolveResource(binding.Resource, resource) || resource.Buffer == VK_NULL_HANDLE || resource.BufferDeviceAddress == 0)
	{
		return 0;
	}

	return resource.BufferDeviceAddress + binding.OffsetInBytes;
}
