#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Device/VulkanRayTracingFeatureQuery.h"
#include "Vulkan/Device/VulkanDeviceExtensions.h"

VulkanRayTracingFeatureStatus VulkanRayTracingFeatureQuery::Query(VkPhysicalDevice physicalDevice) noexcept
{
	VulkanRayTracingFeatureStatus status{};
	if (physicalDevice == VK_NULL_HANDLE)
	{
		return status;
	}

	status.bSupportsAccelerationStructureExtension = VulkanDeviceExtensions::IsAvailable(physicalDevice, VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
	status.bSupportsRayTracingPipelineExtension = VulkanDeviceExtensions::IsAvailable(physicalDevice, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
	status.bSupportsRayQueryExtension = VulkanDeviceExtensions::IsAvailable(physicalDevice, VK_KHR_RAY_QUERY_EXTENSION_NAME);
	status.bSupportsDeferredHostOperationsExtension = VulkanDeviceExtensions::IsAvailable(physicalDevice, VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
	status.bSupportsBufferDeviceAddressExtension = VulkanDeviceExtensions::IsAvailable(physicalDevice, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
	status.bSupportsPtlasExtension = VulkanDeviceExtensions::IsAvailable(physicalDevice, VK_NV_PARTITIONED_ACCELERATION_STRUCTURE_EXTENSION_NAME);

	VkPhysicalDeviceFeatures2 features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
	VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES};
	VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR};
	VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipelineFeatures{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR};
	VkPhysicalDeviceRayQueryFeaturesKHR rayQueryFeatures{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR};
	VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV ptlasFeatures{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PARTITIONED_ACCELERATION_STRUCTURE_FEATURES_NV};

	void** next = &features.pNext;
	*next = &bufferDeviceAddressFeatures;
	next = &bufferDeviceAddressFeatures.pNext;
	if (status.bSupportsAccelerationStructureExtension)
	{
		*next = &accelerationStructureFeatures;
		next = &accelerationStructureFeatures.pNext;
	}
	if (status.bSupportsRayTracingPipelineExtension)
	{
		*next = &rayTracingPipelineFeatures;
		next = &rayTracingPipelineFeatures.pNext;
	}
	if (status.bSupportsRayQueryExtension)
	{
		*next = &rayQueryFeatures;
		next = &rayQueryFeatures.pNext;
	}
	if (status.bSupportsPtlasExtension)
	{
		*next = &ptlasFeatures;
	}

	vkGetPhysicalDeviceFeatures2(physicalDevice, &features);
	status.bSupportsBufferDeviceAddressFeature = bufferDeviceAddressFeatures.bufferDeviceAddress == VK_TRUE;
	status.bSupportsAccelerationStructureFeature = accelerationStructureFeatures.accelerationStructure == VK_TRUE;
	status.bSupportsRayTracingPipelineFeature = rayTracingPipelineFeatures.rayTracingPipeline == VK_TRUE;
	status.bSupportsRayQueryFeature = rayQueryFeatures.rayQuery == VK_TRUE;
	status.bSupportsPtlasFeature = ptlasFeatures.partitionedAccelerationStructure == VK_TRUE;

	const bool bAsExtensionsAvailable = status.bSupportsAccelerationStructureExtension && status.bSupportsDeferredHostOperationsExtension;
	const bool bAsFeaturesAvailable = status.bSupportsBufferDeviceAddressFeature && status.bSupportsAccelerationStructureFeature;
	status.bEnabledAccelerationStructure = bAsExtensionsAvailable && bAsFeaturesAvailable;

	const bool bRayQueryAvailable = status.bSupportsRayQueryExtension && status.bSupportsRayQueryFeature;
	const bool bRayPipelineAvailable = status.bSupportsRayTracingPipelineExtension && status.bSupportsRayTracingPipelineFeature;
	status.bEnabledInlineRayQuery = status.bEnabledAccelerationStructure && bRayQueryAvailable;
	status.bEnabledRayTracingPipeline = status.bEnabledAccelerationStructure && bRayPipelineAvailable;
	return status;
}
