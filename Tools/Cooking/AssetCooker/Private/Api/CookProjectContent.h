#pragma once

#include "AssetCookerTypes.h"

#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

struct AssetCookerRequest final
{
	std::filesystem::path workspaceRoot;
	std::string projectName;
	std::string configuration = "DevelopmentGame";
	std::string toolProfile;
	AssetCookerCategory category = AssetCookerCategory::All;
};

struct AssetCookerResult final
{
	int exitCode = 1;
	std::vector<AssetCookerDiagnosticRecord> diagnostics;
	std::size_t productCount = 0;
};

AssetCookerResult CookProjectContent(const AssetCookerRequest& request);
