#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

enum class AssetCookerCategory : std::uint8_t
{
	All = 0,
	Shaders,
	Textures,
	SceneAssets,
	Meshes,
	Materials
};

struct AssetCookerRequest final
{
	std::filesystem::path workspaceRoot;
	std::string projectName;
	std::string configuration = "DevelopmentGame";
	std::string toolProfile;
	AssetCookerCategory category = AssetCookerCategory::All;
};

struct AssetCookerDiagnosticRecord final
{
	AssetCookerCategory category = AssetCookerCategory::All;
	std::string message;
	std::string sourcePath;
};

struct AssetCookerOutputRecord final
{
	AssetCookerCategory category = AssetCookerCategory::All;
	std::string assetId;
	std::string path;
};

struct AssetCookerResult final
{
	int exitCode = 1;
	std::vector<AssetCookerDiagnosticRecord> diagnostics;
	std::vector<AssetCookerOutputRecord> outputs;
};
