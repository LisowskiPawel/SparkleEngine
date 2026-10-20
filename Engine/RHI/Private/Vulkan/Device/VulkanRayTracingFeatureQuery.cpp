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

	status.SupportsAccelerationStructureExtension = VulkanDeviceExtensions::IsAvailable(physicalDevice, VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
	status.SupportsRayTracingPipelineExtension = VulkanDeviceExtensions::IsAvailable(physicalDevice, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
	status.SupportsRayQueryExtension = VulkanDeviceExtensions::IsAvailable(physicalDevice, VK_KHR_RAY_QUERY_EXTENSION_NAME);
	status.SupportsDeferredHostOperationsExtension = VulkanDeviceExtensions::IsAvailable(physicalDevice, VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
	status.SupportsBufferDeviceAddressExtension = VulkanDeviceExtensions::IsAvailable(physicalDevice, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
	status.SupportsPartitionedAccelerationStructureExtension = VulkanDeviceExtensions::IsAvailable(physicalDevice, VK_NV_PARTITIONED_ACCELERATION_STRUCTURE_EXTENSION_NAME);

	VkPhysicalDeviceFeatures2 features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
	VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES};
	VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR};
	VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipelineFeatures{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR};
	VkPhysicalDeviceRayQueryFeaturesKHR rayQueryFeatures{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR};
	VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV partitionedAccelerationStructureFeatures{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PARTITIONED_ACCELERATION_STRUCTURE_FEATURES_NV};

	void** next = &features.pNext;
	*next = &bufferDeviceAddressFeatures;
	next = &bufferDeviceAddressFeatures.pNext;
	if (status.SupportsAccelerationStructureExtension)
	{
		*next = &accelerationStructureFeatures;
		next = &accelerationStructureFeatures.pNext;
	}
	if (status.SupportsRayTracingPipelineExtension)
	{
		*next = &rayTracingPipelineFeatures;
		next = &rayTracingPipelineFeatures.pNext;
	}
	if (status.SupportsRayQueryExtension)
	{
		*next = &rayQueryFeatures;
		next = &rayQueryFeatures.pNext;
	}
	if (status.SupportsPartitionedAccelerationStructureExtension)
	{
		*next = &partitionedAccelerationStructureFeatures;
	}

	vkGetPhysicalDeviceFeatures2(physicalDevice, &features);
	status.SupportsBufferDeviceAddressFeature = bufferDeviceAddressFeatures.bufferDeviceAddress == VK_TRUE;
	status.SupportsAccelerationStructureFeature = accelerationStructureFeatures.accelerationStructure == VK_TRUE;
	status.SupportsRayTracingPipelineFeature = rayTracingPipelineFeatures.rayTracingPipeline == VK_TRUE;
	status.SupportsRayQueryFeature = rayQueryFeatures.rayQuery == VK_TRUE;
	status.SupportsPartitionedAccelerationStructureFeature = partitionedAccelerationStructureFeatures.partitionedAccelerationStructure == VK_TRUE;

	status.EnabledAccelerationStructure = status.SupportsAccelerationStructureExtension && status.SupportsDeferredHostOperationsExtension && status.SupportsBufferDeviceAddressFeature
	    && status.SupportsAccelerationStructureFeature;

	status.EnabledInlineRayQuery = status.EnabledAccelerationStructure && status.SupportsRayQueryExtension && status.SupportsRayQueryFeature;
	status.EnabledRayTracingPipeline = status.EnabledAccelerationStructure && status.SupportsRayTracingPipelineExtension && status.SupportsRayTracingPipelineFeature;
	return status;
}
