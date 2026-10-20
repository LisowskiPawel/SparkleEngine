#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Device/VulkanDeviceExtensions.h"

#include "Vulkan/Core/VulkanResult.h"

#include <algorithm>
#include <string_view>
#include <vector>

namespace VulkanDeviceExtensions
{
	bool IsAvailable(VkPhysicalDevice device, const char* extensionName) noexcept
	{
		std::uint32_t extensionCount = 0;
		if (!VulkanResult::Succeeded(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr)))
		{
			return false;
		}

		std::vector<VkExtensionProperties> extensions(extensionCount);
		if (!VulkanResult::Succeeded(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data())))
		{
			return false;
		}

		return std::any_of(
		    extensions.begin(),
		    extensions.end(),
		    [extensionName](const VkExtensionProperties& extension) noexcept { return std::string_view(extension.extensionName) == extensionName; });
	}
}
