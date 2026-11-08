#include "Vulkan/VulkanPCH.h"

#include "Vulkan/RayTracing/VulkanAccelerationStructureCommands.h"
#include "Vulkan/VulkanTypeConversions.h"

static void RecordMemoryBarrier(VkCommandBuffer commandBuffer, const VkMemoryBarrier2& barrier) noexcept
{
	const VkDependencyInfo dependencyInfo{
	    .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
	    .pNext = nullptr,
	    .dependencyFlags = 0,
	    .memoryBarrierCount = 1,
	    .pMemoryBarriers = &barrier,
	    .bufferMemoryBarrierCount = 0,
	    .pBufferMemoryBarriers = nullptr,
	    .imageMemoryBarrierCount = 0,
	    .pImageMemoryBarriers = nullptr};

	vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
}

void VulkanAccelerationStructureCommands::BuildBlas(
    VkCommandBuffer commandBuffer,
    PFN_vkCmdBuildAccelerationStructuresKHR build,
    const RhiRayTracingGeometryDesc& geometry,
    VkDeviceAddress vertexBufferAddress,
    VkDeviceAddress indexBufferAddress,
    VkDeviceAddress scratchGpuAddress,
    VkAccelerationStructureKHR destination) noexcept
{
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
	    .dstAccelerationStructure = destination,
	    .geometryCount = 1,
	    .pGeometries = &nativeGeometry,
	    .ppGeometries = nullptr,
	    .scratchData = VkDeviceOrHostAddressKHR{.deviceAddress = scratchGpuAddress}};

	const std::uint32_t primitiveCount = geometry.IndexCount / 3u;
	const VkAccelerationStructureBuildRangeInfoKHR rangeInfo{.primitiveCount = primitiveCount, .primitiveOffset = 0, .firstVertex = 0, .transformOffset = 0};
	const VkAccelerationStructureBuildRangeInfoKHR* rangeInfos[] = {&rangeInfo};
	build(commandBuffer, 1, &buildInfo, rangeInfos);

	const VkMemoryBarrier2 buildBarrier{
	    .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
	    .pNext = nullptr,
	    .srcStageMask = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
	    .srcAccessMask = VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR,
	    .dstStageMask = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
	    .dstAccessMask = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR};

	RecordMemoryBarrier(commandBuffer, buildBarrier);
}

void VulkanAccelerationStructureCommands::BuildTlas(
    VkCommandBuffer commandBuffer,
    PFN_vkCmdBuildAccelerationStructuresKHR build,
    VkDeviceAddress instanceDescsGpuAddress,
    std::uint32_t instanceCount,
    VkDeviceAddress scratchGpuAddress,
    VkAccelerationStructureKHR destination,
    ERhiTlasBuildMode buildMode) noexcept
{
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
	    .srcAccelerationStructure = buildMode == ERhiTlasBuildMode::Update ? destination : VK_NULL_HANDLE,
	    .dstAccelerationStructure = destination,
	    .geometryCount = 1,
	    .pGeometries = &geometry,
	    .ppGeometries = nullptr,

	    .scratchData = VkDeviceOrHostAddressKHR{.deviceAddress = scratchGpuAddress}};

	const VkAccelerationStructureBuildRangeInfoKHR rangeInfo{.primitiveCount = instanceCount, .primitiveOffset = 0, .firstVertex = 0, .transformOffset = 0};
	const VkAccelerationStructureBuildRangeInfoKHR* rangeInfos[] = {&rangeInfo};
	build(commandBuffer, 1, &buildInfo, rangeInfos);

	const VkMemoryBarrier2 buildBarrier{
	    .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
	    .pNext = nullptr,
	    .srcStageMask = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
	    .srcAccessMask = VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR,
	    .dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
	    .dstAccessMask = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR};

	RecordMemoryBarrier(commandBuffer, buildBarrier);
}

void VulkanAccelerationStructureCommands::BuildPtlas(VkCommandBuffer commandBuffer, PFN_vkCmdBuildPartitionedAccelerationStructuresNV build, const RhiPtlasBuildCommandDesc& desc) noexcept
{
	const VkMemoryBarrier2 operationDataBarrier{
	    .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
	    .pNext = nullptr,
	    .srcStageMask = VK_PIPELINE_STAGE_2_HOST_BIT | VK_PIPELINE_STAGE_2_TRANSFER_BIT | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
	    .srcAccessMask = VK_ACCESS_2_HOST_WRITE_BIT | VK_ACCESS_2_TRANSFER_WRITE_BIT | VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,
	    .dstStageMask = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
	    .dstAccessMask = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR};

	RecordMemoryBarrier(commandBuffer, operationDataBarrier);

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

	build(commandBuffer, &buildInfo);

	const VkMemoryBarrier2 buildBarrier{
	    .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
	    .pNext = nullptr,
	    .srcStageMask = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
	    .srcAccessMask = VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR,
	    .dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
	    .dstAccessMask = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_2_SHADER_SAMPLED_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_READ_BIT};

	RecordMemoryBarrier(commandBuffer, buildBarrier);
}
