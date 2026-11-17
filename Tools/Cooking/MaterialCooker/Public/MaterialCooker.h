#pragma once

#include <string_view>
#include <vector>

namespace Files
{
	struct FilePublication;
}

struct CookedMaterialAssetBuild;
struct SourceImportOutput;
class TextureCookRequestSet;

namespace MaterialCooker
{
	std::vector<CookedMaterialAssetBuild> BuildMaterialAssets(const SourceImportOutput& importOutput, std::string_view sceneAssetId);
	void AppendTextureCookRequests(const SourceImportOutput& importOutput, TextureCookRequestSet& requests);
	void StageMaterialAssets(const std::vector<CookedMaterialAssetBuild>& materialAssets, std::vector<Files::FilePublication>& outPublication);
}
