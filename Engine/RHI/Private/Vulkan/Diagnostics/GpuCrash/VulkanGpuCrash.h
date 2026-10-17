#pragma once

#include "Vulkan/VulkanIncludes.h"
#include "Diagnostics/GpuCrash/GpuCrashSession.h"
#include "Commands/RhiQueue.h"

#include <vector>

class VulkanRhi;

class VulkanGpuCrashConfiguration final
{
public:
	VulkanGpuCrashConfiguration(VkPhysicalDevice physicalDevice, std::vector<const char*>& extensions);
	void Attach(VkDeviceCreateInfo& device, VkPhysicalDeviceFeatures2& features) noexcept;

private:
	bool m_enabled = false;
	VkPhysicalDeviceDiagnosticsConfigFeaturesNV m_features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DIAGNOSTICS_CONFIG_FEATURES_NV};
	VkDeviceDiagnosticsConfigCreateInfoNV m_config{VK_STRUCTURE_TYPE_DEVICE_DIAGNOSTICS_CONFIG_CREATE_INFO_NV};
};

namespace VulkanGpuCrash
{
	void Initialize(VkDevice device);
	void Checkpoint(VkCommandBuffer commands, const void* token) noexcept;
	void Shader(std::span<const std::byte> code) noexcept;
	void ObserveLoss(VulkanRhi& rhi, ERhiQueueType observedQueue, VkResult result) noexcept;
}
