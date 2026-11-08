#pragma once

#include <string_view>
#include <vector>

namespace Files
{
	struct FilePublication;
}

struct CookedMaterialAssetBuild;
struct MaterialCookOutput;
struct SourceImportOutput;
struct TextureCookRequest;

namespace MaterialCooker
{
	MaterialCookOutput BuildMaterialAssets(const SourceImportOutput& importOutput, std::string_view sceneAssetId);
	std::vector<TextureCookRequest> CollectTextureCookRequests(const SourceImportOutput& importOutput);
	void StageMaterialAssets(const std::vector<CookedMaterialAssetBuild>& materialAssets, std::vector<Files::FilePublication>& outPublication);
}
