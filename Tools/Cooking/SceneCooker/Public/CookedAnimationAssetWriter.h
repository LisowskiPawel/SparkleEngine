#pragma once

#include <vector>

namespace Files
{
	struct FilePublication;
}

struct CookedAnimationAssetBuild;

namespace CookedAnimationAssetWriter
{
	void StageAnimationAssets(const std::vector<CookedAnimationAssetBuild>& animationAssets, std::vector<Files::FilePublication>& outPublication);
}
