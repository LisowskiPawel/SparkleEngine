#pragma once

#include <cstdint>
#include <string>

enum class AssetCookerCategory : std::uint8_t
{
	All = 0,
	Shaders,
	Textures,
	SceneAssets,
	Meshes,
	Materials
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
