#pragma once

#include "Vulkan/VulkanIncludes.h"

#include <cstdint>
#include <string>
#include <vector>

class VulkanQueueTopology;
struct VulkanFeatureStatus;

VkDevice CreateVulkanLogicalDevice(
    VkPhysicalDevice physicalDevice,
    const VulkanQueueTopology& topology,
    std::uint32_t apiVersion,
    bool bNvidiaAdapter,
    VulkanFeatureStatus& status,
    std::vector<std::string>& enabledExtensions) noexcept;
