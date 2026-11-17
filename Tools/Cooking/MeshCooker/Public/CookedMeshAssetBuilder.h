#pragma once

#include "CookedMeshAssetBuild.h"

#include <string_view>

struct SourceImportOutput;

class CookedMeshAssetBuilder final
{
public:
	static std::vector<CookedMeshAssetBuild> BuildMeshAssets(const SourceImportOutput& importOutput, std::string_view sceneAssetId);
};
