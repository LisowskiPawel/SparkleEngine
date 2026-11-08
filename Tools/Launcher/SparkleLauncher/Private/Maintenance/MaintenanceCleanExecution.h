#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace SparkleLauncher
{
	struct MaintenanceCleanStep;
	bool RunCleanStep(const MaintenanceCleanStep& step, const std::vector<std::filesystem::path>& preservedPaths, std::string& outErrorMessage);
}
