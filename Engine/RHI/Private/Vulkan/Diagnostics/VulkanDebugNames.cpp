#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Diagnostics/VulkanDebugNames.h"

bool VulkanDebugNames::SetObjectName(PFN_vkSetDebugUtilsObjectNameEXT setObjectName, VkDevice device, VkObjectType objectType, std::uint64_t objectHandle, std::string_view name) noexcept
{
	if (setObjectName == nullptr || device == VK_NULL_HANDLE || objectHandle == 0 || name.empty())
	{
		return false;
	}

	const std::string objectName{name};
	VkDebugUtilsObjectNameInfoEXT nameInfo{};
	nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	nameInfo.objectType = objectType;
	nameInfo.objectHandle = objectHandle;
	nameInfo.pObjectName = objectName.c_str();
	return setObjectName(device, &nameInfo) == VK_SUCCESS;
}
