#pragma once

#include <filesystem>

struct CookedSkeletonAssetBuild;

namespace CookedSkeletonAssetWriter
{
	void Write(const CookedSkeletonAssetBuild& asset, const std::filesystem::path& outputPath);
}
