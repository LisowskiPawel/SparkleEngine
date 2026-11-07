#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace SparkleLauncher
{
	struct VulkanSdkStatus final
	{
		bool bAvailable = false;
		std::filesystem::path Root;
		std::string Detail;
	};

	std::optional<std::filesystem::path> DetectInstalledVulkanSdkRoot();
	VulkanSdkStatus DetectVulkanSdk();
}
