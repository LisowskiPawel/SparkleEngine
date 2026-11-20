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
		const VkResult countResult = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
		if (!VulkanResult::Succeeded(countResult))
		{
			return false;
		}

		std::vector<VkExtensionProperties> extensions(extensionCount);
		const VkResult propertiesResult = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());
		if (!VulkanResult::Succeeded(propertiesResult))
		{
			return false;
		}

		return std::any_of(
		    extensions.begin(),
		    extensions.end(),
		    [extensionName](const VkExtensionProperties& extension) noexcept { return std::string_view(extension.extensionName) == extensionName; });
	}

	bool AppendIfAvailable(VkPhysicalDevice device, std::vector<const char*>& extensions, const char* extensionName) noexcept
	{
		const bool bAlreadyEnabled = extensionName == nullptr
		    || std::any_of(extensions.begin(), extensions.end(), [extensionName](const char* enabled) noexcept { return std::string_view(enabled) == extensionName; });

		if (bAlreadyEnabled || !IsAvailable(device, extensionName))
		{
			return false;
		}
		extensions.push_back(extensionName);
		return true;
	}

}
