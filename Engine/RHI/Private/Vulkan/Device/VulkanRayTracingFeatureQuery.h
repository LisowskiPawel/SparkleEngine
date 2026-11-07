#pragma once

#include "Vulkan/VulkanIncludes.h"

struct VulkanRayTracingFeatureStatus final
{
	bool bSupportsAccelerationStructureExtension = false;
	bool bSupportsRayTracingPipelineExtension = false;
	bool bSupportsRayQueryExtension = false;
	bool bSupportsDeferredHostOperationsExtension = false;
	bool bSupportsBufferDeviceAddressExtension = false;
	bool bSupportsPtlasExtension = false;
	bool bSupportsBufferDeviceAddressFeature = false;
	bool bSupportsAccelerationStructureFeature = false;
	bool bSupportsRayTracingPipelineFeature = false;
	bool bSupportsRayQueryFeature = false;
	bool bSupportsPtlasFeature = false;
	bool bEnabledAccelerationStructure = false;
	bool bEnabledInlineRayQuery = false;
	bool bEnabledRayTracingPipeline = false;
	bool bEnabledPtlas = false;
};

namespace VulkanRayTracingFeatureQuery
{
	VulkanRayTracingFeatureStatus Query(VkPhysicalDevice physicalDevice) noexcept;
}
