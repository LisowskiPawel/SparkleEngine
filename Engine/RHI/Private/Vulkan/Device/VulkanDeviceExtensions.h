#pragma once

#include "Vulkan/VulkanIncludes.h"

#include <vector>

namespace VulkanDeviceExtensions
{
	bool AppendIfAvailable(VkPhysicalDevice device, std::vector<const char*>& extensions, const char* extensionName) noexcept;
	bool IsAvailable(VkPhysicalDevice device, const char* extensionName) noexcept;
}
