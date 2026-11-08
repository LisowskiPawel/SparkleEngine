#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Device/VulkanDeviceCreation.h"
#include "Vulkan/Device/VulkanDeviceExtensions.h"
#include "Vulkan/Device/VulkanRhi.h"
#include "Vulkan/Core/VulkanResult.h"
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
  #include "Vulkan/Diagnostics/GpuCrash/VulkanGpuCrash.h"
#endif

#include <vector>
#include <span>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_vulkanDeviceLogger, "RHI.Vulkan");

static std::vector<VkDeviceQueueCreateInfo> BuildQueueCreateInfos(const VulkanQueueTopology& topology, std::vector<std::vector<float>>& queuePriorities)
{
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	const std::span<const VulkanQueueFamilyRequest> familyRequests = topology.GetFamilyRequests();
	queueCreateInfos.reserve(familyRequests.size());
	queuePriorities.reserve(familyRequests.size());
	for (const VulkanQueueFamilyRequest& familyRequest : familyRequests)
	{
		queuePriorities.emplace_back(familyRequest.QueueCount, 1.0f);

		queueCreateInfos.push_back(
		    VkDeviceQueueCreateInfo{
		        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		        .pNext = nullptr,
		        .flags = 0,
		        .queueFamilyIndex = familyRequest.FamilyIndex,
		        .queueCount = familyRequest.QueueCount,
		        .pQueuePriorities = queuePriorities.back().data()});
	}

	return queueCreateInfos;
}

static std::vector<const char*> SelectDeviceExtensions(VkPhysicalDevice physicalDevice, std::uint32_t apiVersion, bool bNvidiaAdapter, VulkanFeatureStatus& status)
{
	std::vector<const char*> deviceExtensions;
	if (VulkanDeviceExtensions::IsAvailable(physicalDevice, VK_KHR_SWAPCHAIN_EXTENSION_NAME))
	{
		deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}
	if (VulkanDeviceExtensions::IsAvailable(physicalDevice, VK_EXT_MEMORY_BUDGET_EXTENSION_NAME))
	{
		deviceExtensions.push_back(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
	}
	if (status.RayTracing.bEnabledAccelerationStructure)
	{
		deviceExtensions.push_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
		deviceExtensions.push_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
		if (status.RayTracing.bEnabledInlineRayQuery)
		{
			deviceExtensions.push_back(VK_KHR_RAY_QUERY_EXTENSION_NAME);
		}
		if (status.RayTracing.bEnabledRayTracingPipeline)
		{
			deviceExtensions.push_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
		}
		if (apiVersion < VK_API_VERSION_1_2 && status.RayTracing.bSupportsBufferDeviceAddressExtension)
		{
			deviceExtensions.push_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
		}
		if (bNvidiaAdapter && status.RayTracing.bSupportsPtlasExtension && status.RayTracing.bSupportsPtlasFeature)
		{
			deviceExtensions.push_back(VK_NV_PARTITIONED_ACCELERATION_STRUCTURE_EXTENSION_NAME);
			status.RayTracing.bEnabledPtlas = true;
		}
	}
	VulkanDeviceExtensions::AppendIfAvailable(physicalDevice, deviceExtensions, "VK_NVX_binary_import");
	VulkanDeviceExtensions::AppendIfAvailable(physicalDevice, deviceExtensions, "VK_NVX_image_view_handle");
	return deviceExtensions;
}

// The feature chain points into this assembly; it must stay at one address through device creation.
class VulkanDeviceFeatures final
{
public:
	VulkanDeviceFeatures(std::uint32_t apiVersion, VulkanFeatureStatus& status) noexcept
	{
		EnableCoreFeatures(status);
		EnableVulkan12Features(apiVersion, status);
		EnableVulkan13Features(apiVersion, status);
		EnableRayTracingFeatures(apiVersion, status);
	}

	VulkanDeviceFeatures(const VulkanDeviceFeatures&) = delete;
	VulkanDeviceFeatures& operator=(const VulkanDeviceFeatures&) = delete;
	VulkanDeviceFeatures(VulkanDeviceFeatures&&) = delete;
	VulkanDeviceFeatures& operator=(VulkanDeviceFeatures&&) = delete;

	VkPhysicalDeviceFeatures2& Get() noexcept { return m_features; }

private:
	void EnableCoreFeatures(VulkanFeatureStatus& status) noexcept
	{
		m_features.features.samplerAnisotropy = status.bSupportsSamplerAnisotropy ? VK_TRUE : VK_FALSE;
		m_features.features.fillModeNonSolid = status.bSupportsFillModeNonSolid ? VK_TRUE : VK_FALSE;
		m_features.features.shaderInt64 = status.bSupportsShaderInt64 ? VK_TRUE : VK_FALSE;
		m_features.features.shaderFloat64 = status.bSupportsShaderFloat64 ? VK_TRUE : VK_FALSE;
		m_features.features.shaderStorageImageReadWithoutFormat = status.bSupportsStorageImageReadWithoutFormat ? VK_TRUE : VK_FALSE;
		m_features.features.shaderStorageImageWriteWithoutFormat = status.bSupportsStorageImageWriteWithoutFormat ? VK_TRUE : VK_FALSE;
		status.bEnabledSamplerAnisotropy = m_features.features.samplerAnisotropy == VK_TRUE;
		status.bEnabledFillModeNonSolid = m_features.features.fillModeNonSolid == VK_TRUE;
		status.bEnabledShaderInt64 = m_features.features.shaderInt64 == VK_TRUE;
		status.bEnabledShaderFloat64 = m_features.features.shaderFloat64 == VK_TRUE;
		status.bEnabledStorageImageReadWithoutFormat = m_features.features.shaderStorageImageReadWithoutFormat == VK_TRUE;
		status.bEnabledStorageImageWriteWithoutFormat = m_features.features.shaderStorageImageWriteWithoutFormat == VK_TRUE;
	}

	void EnableVulkan12Features(std::uint32_t apiVersion, VulkanFeatureStatus& status) noexcept
	{
		if (apiVersion >= VK_API_VERSION_1_2)
		{
			m_features12.scalarBlockLayout = status.bSupportsScalarBlockLayout ? VK_TRUE : VK_FALSE;
			status.bEnabledScalarBlockLayout = m_features12.scalarBlockLayout == VK_TRUE;
			m_features12.hostQueryReset = status.bSupportsHostQueryReset ? VK_TRUE : VK_FALSE;
			status.bEnabledHostQueryReset = m_features12.hostQueryReset == VK_TRUE;
			m_features12.timelineSemaphore = status.bSupportsTimelineSemaphore ? VK_TRUE : VK_FALSE;
			m_features12.shaderSampledImageArrayNonUniformIndexing = status.bNonUniformSampledImages ? VK_TRUE : VK_FALSE;
			m_features12.descriptorBindingPartiallyBound = status.bSupportsPartiallyBoundDescriptorArrays ? VK_TRUE : VK_FALSE;
			*m_next = &m_features12;
			m_next = &m_features12.pNext;
			status.bNonUniformSampledImagesEnabled = m_features12.shaderSampledImageArrayNonUniformIndexing == VK_TRUE;
			status.bEnabledPartiallyBoundDescriptorArrays = m_features12.descriptorBindingPartiallyBound == VK_TRUE;
			status.bEnabledTimelineSemaphore = m_features12.timelineSemaphore == VK_TRUE;
		}
	}

	void EnableVulkan13Features(std::uint32_t apiVersion, VulkanFeatureStatus& status) noexcept
	{
		if (apiVersion >= VK_API_VERSION_1_3)
		{
			m_features13.synchronization2 = status.bSupportsSynchronization2 ? VK_TRUE : VK_FALSE;
			m_features13.dynamicRendering = status.bSupportsDynamicRendering ? VK_TRUE : VK_FALSE;
			m_features13.shaderDemoteToHelperInvocation = status.bSupportsShaderDemoteToHelperInvocation ? VK_TRUE : VK_FALSE;
			*m_next = &m_features13;
			m_next = &m_features13.pNext;
			status.bEnabledSynchronization2 = m_features13.synchronization2 == VK_TRUE;
			status.bEnabledDynamicRendering = m_features13.dynamicRendering == VK_TRUE;

			status.bEnabledShaderDemoteToHelperInvocation = m_features13.shaderDemoteToHelperInvocation == VK_TRUE;
		}
	}

	void EnableRayTracingFeatures(std::uint32_t apiVersion, VulkanFeatureStatus& status) noexcept
	{
		if (status.RayTracing.bEnabledAccelerationStructure)
		{
			if (apiVersion >= VK_API_VERSION_1_2)
			{
				m_features12.bufferDeviceAddress = VK_TRUE;
			}
			else
			{
				m_bufferDeviceAddress.bufferDeviceAddress = VK_TRUE;
				*m_next = &m_bufferDeviceAddress;
				m_next = &m_bufferDeviceAddress.pNext;
			}
			m_accelerationStructure.accelerationStructure = VK_TRUE;
			*m_next = &m_accelerationStructure;
			m_next = &m_accelerationStructure.pNext;
			if (status.RayTracing.bEnabledRayTracingPipeline)
			{
				m_rayTracingPipeline.rayTracingPipeline = VK_TRUE;
				*m_next = &m_rayTracingPipeline;
				m_next = &m_rayTracingPipeline.pNext;
			}
			if (status.RayTracing.bEnabledInlineRayQuery)
			{
				m_rayQuery.rayQuery = VK_TRUE;
				*m_next = &m_rayQuery;
				m_next = &m_rayQuery.pNext;
			}
			if (status.RayTracing.bEnabledPtlas)
			{
				m_ptlas.partitionedAccelerationStructure = VK_TRUE;
				*m_next = &m_ptlas;
				m_next = &m_ptlas.pNext;
			}
		}
	}

	VkPhysicalDeviceFeatures2 m_features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
	VkPhysicalDeviceVulkan12Features m_features12{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
	VkPhysicalDeviceVulkan13Features m_features13{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
	VkPhysicalDeviceAccelerationStructureFeaturesKHR m_accelerationStructure{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR};
	VkPhysicalDeviceBufferDeviceAddressFeatures m_bufferDeviceAddress{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES};
	VkPhysicalDeviceRayTracingPipelineFeaturesKHR m_rayTracingPipeline{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR};
	VkPhysicalDeviceRayQueryFeaturesKHR m_rayQuery{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR};
	VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV m_ptlas{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PARTITIONED_ACCELERATION_STRUCTURE_FEATURES_NV};
	void** m_next = &m_features.pNext;
};

VkDevice CreateVulkanLogicalDevice(
    VkPhysicalDevice physicalDevice,
    const VulkanQueueTopology& topology,
    std::uint32_t apiVersion,
    bool bNvidiaAdapter,
    VulkanFeatureStatus& status,
    std::vector<std::string>& enabledExtensions) noexcept
{
	std::vector<std::vector<float>> queuePriorities;
	const std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = BuildQueueCreateInfos(topology, queuePriorities);
	std::vector<const char*> deviceExtensions = SelectDeviceExtensions(physicalDevice, apiVersion, bNvidiaAdapter, status);
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
	VulkanGpuCrashConfiguration gpuCrash(physicalDevice, deviceExtensions);
#endif
	for (const char* extension : deviceExtensions)
	{
		enabledExtensions.emplace_back(extension);
	}

	VulkanDeviceFeatures features(apiVersion, status);
	VkDevice device = VK_NULL_HANDLE;

	VkDeviceCreateInfo createInfo{
	    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
	    .pNext = &features.Get(),
	    .flags = 0,
	    .queueCreateInfoCount = static_cast<std::uint32_t>(queueCreateInfos.size()),
	    .pQueueCreateInfos = queueCreateInfos.data(),
	    .enabledLayerCount = 0,
	    .ppEnabledLayerNames = nullptr,
	    .enabledExtensionCount = static_cast<std::uint32_t>(deviceExtensions.size()),
	    .ppEnabledExtensionNames = deviceExtensions.empty() ? nullptr : deviceExtensions.data(),
	    .pEnabledFeatures = nullptr};
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
	gpuCrash.Attach(createInfo, features.Get());
#endif

	const VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
	if (!VulkanResult::Succeeded(result))
	{
		Diagnostics::Fatal(g_vulkanDeviceLogger, __FILE__, __LINE__, VulkanResult::FormatFailure("vkCreateDevice", result));
	}

	return device;
}
