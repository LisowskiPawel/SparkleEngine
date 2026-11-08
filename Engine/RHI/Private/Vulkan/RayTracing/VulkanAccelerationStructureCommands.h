#pragma once

#include "RayTracing/RhiRayTracingDesc.h"
#include "RayTracing/RhiTlasDesc.h"
#include "RayTracing/RhiPtlasDesc.h"
#include "Vulkan/VulkanIncludes.h"

#include <cstdint>

namespace VulkanAccelerationStructureCommands
{
	void BuildBlas(
	    VkCommandBuffer commandBuffer,
	    PFN_vkCmdBuildAccelerationStructuresKHR build,
	    const RhiRayTracingGeometryDesc& geometry,
	    VkDeviceAddress vertexBufferAddress,
	    VkDeviceAddress indexBufferAddress,
	    VkDeviceAddress scratchGpuAddress,
	    VkAccelerationStructureKHR destination) noexcept;
	void BuildTlas(
	    VkCommandBuffer commandBuffer,
	    PFN_vkCmdBuildAccelerationStructuresKHR build,
	    VkDeviceAddress instanceDescsGpuAddress,
	    std::uint32_t instanceCount,
	    VkDeviceAddress scratchGpuAddress,
	    VkAccelerationStructureKHR destination,
	    ERhiTlasBuildMode buildMode) noexcept;
	void BuildPtlas(VkCommandBuffer commandBuffer, PFN_vkCmdBuildPartitionedAccelerationStructuresNV build, const RhiPtlasBuildCommandDesc& desc) noexcept;
}
