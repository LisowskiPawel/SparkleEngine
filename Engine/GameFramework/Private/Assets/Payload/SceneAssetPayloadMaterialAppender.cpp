#include "PCH.h"

#include "Assets/Payload/SceneAssetPayloadMaterialAppender.h"

#include "Assets/Cooked/LoadedMaterialAsset.h"
#include "Assets/CookedAssembly/CookedMaterialTranslator.h"
#include "Assets/Loaders/MaterialAssetLoader.h"
#include "Assets/Loaders/CookedAssetAllocationBudget.h"
#include "Assets/Loaders/CookedAssetFileSet.h"
#include "Core/Public/Formatting/HexFormat.h"
#include "Core/Public/Paths/CookedAssetPaths.h"

#include <filesystem>
#include <format>
#include <utility>

namespace Assets
{
	void SceneAssetPayloadMaterialAppender::AppendMaterials(const LoadedSceneManifest& sceneManifest, CookedAssetFileSet& files, SceneAssetPayload& sceneAssetPayload)
	{
		CookedAssetAllocationBudget& budget = files.GetAllocationBudget();
		budget.ReserveArray<MaterialDesc>(sceneManifest.materialAssetReferences.size());
		sceneAssetPayload.materials.reserve(sceneAssetPayload.materials.size() + sceneManifest.materialAssetReferences.size());

		for (const CookedSceneMaterialAssetRef& materialReference : sceneManifest.materialAssetReferences)
		{
			const std::filesystem::path materialAssetPath = Paths::CookedMaterialAsset(materialReference);
			const auto bytes = files.Get(materialAssetPath);
			const LoadedMaterialAsset materialAsset = MaterialAssetLoader::Decode(bytes, budget);
			budget.Reserve(bytes.size());
			MaterialDesc material = CookedMaterialTranslator::Translate(materialAsset);
			sceneAssetPayload.materials.push_back(std::move(material));
			files.Release(materialAssetPath);
		}
	}
}
