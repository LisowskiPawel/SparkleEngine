#pragma once

#include <vector>

namespace Files
{
	struct FilePublication;
}

struct CookedSkeletonAssetBuild;

namespace CookedSkeletonAssetWriter
{
	void StageSkeletonAssets(const std::vector<CookedSkeletonAssetBuild>& skeletonAssets, std::vector<Files::FilePublication>& outPublication);
}
