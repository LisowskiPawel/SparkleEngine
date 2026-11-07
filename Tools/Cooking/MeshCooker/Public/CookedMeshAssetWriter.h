#pragma once

#include <vector>

namespace Files
{
	struct FilePublication;
}

struct CookedMeshAssetBuild;

namespace CookedMeshAssetWriter
{
	void StageMeshAssets(const std::vector<CookedMeshAssetBuild>& meshAssets, std::vector<Files::FilePublication>& outPublication);
}
