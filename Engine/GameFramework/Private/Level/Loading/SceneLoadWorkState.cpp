#include "PCH.h"
#include "Level/Loading/SceneLoadWorkState.h"
#include "Level/Loading/SceneLoadPackageBuilder.h"
#include "Assets/SceneAssetCatalog.h"
#include "Assets/Loading/SceneAssetFileReader.h"
#include "Assets/Loading/SceneAssetPayloadDecoder.h"
#include "Core/Public/Diagnostics/Error.h"

#include <string>
#include <optional>
#include <format>
#include <unordered_set>
#include <utility>

namespace Assets
{
	void SceneLoadWorkState::PreparePackage(std::uint64_t requestId, std::uint64_t worldGeneration, std::uint64_t documentGeneration, LevelDesc level, const SceneAssetCatalog& catalog)
	{
		constexpr std::size_t MaximumAssetsPerRequest = 256;
		if (level.sceneAssetIds.size() > MaximumAssetsPerRequest)
		{
			throw Diagnostics::Error("Level exceeds the bounded scene-asset request capacity.");
		}
		Package = std::make_unique<SceneLoadPackage>();
		Package->RequestId = requestId;
		Package->WorldGeneration = worldGeneration;
		Package->DocumentGeneration = documentGeneration;
		Package->CatalogGeneration = catalog.GetGeneration();
		Package->Level = std::move(level);
		Assets.reserve(Package->Level.sceneAssetIds.size());
		ResolveAssets(catalog);
	}

	void SceneLoadWorkState::ResolveAssets(const SceneAssetCatalog& catalog)
	{
		std::unordered_set<std::string> identities;
		for (const SceneAssetId& id : Package->Level.sceneAssetIds)
		{
			const bool bEmptyIdentity = id.IsEmpty();
			const bool bUniqueIdentity = !bEmptyIdentity && identities.insert(id.value).second;
			if (!bUniqueIdentity)
			{
				const std::string errorMessage = bEmptyIdentity ? "Level contains an empty scene asset identity." : std::format("Level contains duplicate scene asset identity '{}'.", id.value);
				throw Diagnostics::Error(errorMessage);
			}
			const std::optional<std::filesystem::path> manifest = catalog.Resolve(id.GetCatalogValue());
			if (!manifest)
			{
				throw Diagnostics::Error(std::format("Scene asset catalog ID '{}' for instance '{}' is absent from catalog generation {}.", id.GetCatalogValue(), id.value, catalog.GetGeneration()));
			}
			Assets.emplace_back(Publication, AllocationBudget, id, *manifest);
		}
	}

	void SceneLoadWorkState::ReadAsset(std::size_t index)
	{
		SceneAssetLoadWork& work = Assets[index];
		SceneAssetFileReader::Read(work.Id, work.ManifestPath, work.Manifest, work.Files);
	}

	void SceneLoadWorkState::DecodeAsset(std::size_t index)
	{
		SceneAssetLoadWork& work = Assets[index];
		work.Payload = SceneAssetPayloadDecoder::Decode(work.Manifest, work.Files);
		PrepareSceneAssetForCommit(work);
	}
}
