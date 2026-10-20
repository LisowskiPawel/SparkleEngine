#pragma once

#include "Vulkan/VulkanIncludes.h"

namespace VulkanDeviceExtensions
{
	bool IsAvailable(VkPhysicalDevice device, const char* extensionName) noexcept;
}
