#pragma once

#include <filesystem>

struct CookedAnimationAssetBuild;

namespace CookedAnimationAssetWriter
{
	void Write(const CookedAnimationAssetBuild& asset, const std::filesystem::path& outputPath);
}
