#include "PCH.h"
#include "Core/Public/Paths/CookedAssetPaths.h"

#include "MaterialCooker.h"
#include "CookedMaterialAssetBuild.h"
#include "SourceImportOutput.h"
#include "TextureCookRequestBuilder.h"
#include "Types/ImportedMaterial.h"
#include "Types/ImportedTextureSource.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Core/Public/Paths/PathUtils.h"

#include <optional>

static std::string BuildCookedTextureReferencePath(const TextureCookRequest& request)
{
	const std::optional<std::filesystem::path> relativePath = Paths::TryMakeRelativeUnderRoot(request.outputPath, Filesystem::GetCookedAssetRootPath());
	if (!relativePath)
	{
		throw Diagnostics::Error("Cooked texture output path is outside the cooked asset root: '" + request.outputPath.string() + "'.");
	}

	return relativePath->generic_string();
}

static Assets::CookedAssetId BuildMaterialAssetId(std::string_view sceneAssetId, std::size_t materialIndex)
{
	return Hash::Fnv1a64(std::string(sceneAssetId) + "#material#" + std::to_string(materialIndex));
}

static Assets::CookedAlphaMode TranslateAlphaMode(ImportedAlphaMode alphaMode)
{
	switch (alphaMode)
	{
		case ImportedAlphaMode::Opaque:
			return Assets::CookedAlphaMode::Opaque;
		case ImportedAlphaMode::Mask:
			return Assets::CookedAlphaMode::Mask;
		case ImportedAlphaMode::Blend:
			return Assets::CookedAlphaMode::Blend;
	}

	throw Diagnostics::Error("Imported material uses an unsupported alpha mode.");
}

static void AppendTextureReference(const ImportedTextureSource& textureSource, CookedMaterialAssetBuild& materialAsset)
{
	if (textureSource.sourcePath.empty())
	{
		return;
	}

	const TextureCookRequest request = BuildTextureCookRequest(textureSource.sourcePath, textureSource.textureGroup, textureSource.channelMask);

	materialAsset.textureReferences.push_back({BuildCookedTextureReferencePath(request), textureSource.textureGroup, textureSource.mapping});
}

static CookedMaterialAssetBuild BuildMaterialAsset(const ImportedMaterial& importedMaterial, std::string_view sceneAssetId, std::size_t materialIndex)
{
	CookedMaterialAssetBuild materialAsset;
	materialAsset.assetId = BuildMaterialAssetId(sceneAssetId, materialIndex);
	materialAsset.name = importedMaterial.name;
	materialAsset.header.alphaMode = TranslateAlphaMode(importedMaterial.alphaMode);
	materialAsset.header.baseColor = importedMaterial.baseColor;
	materialAsset.header.metallic = importedMaterial.metallic;
	materialAsset.header.roughness = importedMaterial.roughness;
	materialAsset.header.f0 = importedMaterial.f0;
	materialAsset.header.subsurfaceColor = importedMaterial.subsurfaceColor;
	materialAsset.header.subsurfaceStrength = importedMaterial.subsurfaceStrength;
	materialAsset.header.alphaCutoff = importedMaterial.alphaCutoff;
	materialAsset.header.emissiveColor = importedMaterial.emissiveColor;
	materialAsset.header.doubleSided = importedMaterial.bDoubleSided ? 1u : 0u;

	for (const ImportedTextureSource& textureSource : importedMaterial.textureSources)
	{
		AppendTextureReference(textureSource, materialAsset);
	}

	return materialAsset;
}

std::vector<CookedMaterialAssetBuild> MaterialCooker::BuildMaterialAssets(const SourceImportOutput& importOutput, std::string_view sceneAssetId)
{
	std::vector<CookedMaterialAssetBuild> assets;
	assets.reserve(importOutput.scene.materials.size());

	for (std::size_t materialIndex = 0; materialIndex < importOutput.scene.materials.size(); ++materialIndex)
	{
		assets.push_back(BuildMaterialAsset(importOutput.scene.materials[materialIndex], sceneAssetId, materialIndex));
	}

	return assets;
}
