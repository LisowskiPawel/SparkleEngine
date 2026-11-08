#pragma once

#include "SparkleLauncher/MaintenanceOperations.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace SparkleLauncher
{
	enum class MaintenanceCleanBehavior : std::uint8_t
	{
		RemovePath,
		RemoveDirectoryContentsPreservingPath,
		RemoveRootGeneratedFiles
	};

	struct MaintenanceCleanStep
	{
		std::string Id;
		std::string DisplayName;
		std::filesystem::path DestructivePath;
		std::filesystem::path PreservedPath;
		std::string PreviewDetail;
		MaintenanceCleanBehavior CleanBehavior = MaintenanceCleanBehavior::RemovePath;
	};

	std::vector<CleanScope> ResolveRequestedCleanScopes(const MaintenanceOperationRequest& request);
	std::vector<MaintenanceCleanStep> BuildMaintenanceCleanSteps(const MaintenanceOperationPlan& plan);
}
