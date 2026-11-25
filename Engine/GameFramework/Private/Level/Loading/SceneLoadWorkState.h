#pragma once

#include "Level/Loading/SceneLoadCompletion.h"
#include "Level/Loading/SceneLoadPackage.h"
#include "Assets/Cooked/LoadedSceneManifest.h"
#include "Assets/Loaders/CookedAssetFileSet.h"
#include "Core/Public/Files/FileSetPublication.h"

#include <cstdint>
#include <atomic>
#include <filesystem>
#include <memory>
#include <vector>

namespace Assets
{
	class SceneAssetCatalog;

	struct SceneAssetLoadWork final
	{
		SceneAssetLoadWork(const Files::FileSetReadScope& publication, CookedAssetAllocationBudget& budget, const SceneAssetId& id, std::filesystem::path manifestPath) :
		    Id(id),
		    ManifestPath(std::move(manifestPath)),
		    Files(publication, budget)
		{
		}

		SceneAssetId Id;
		std::filesystem::path ManifestPath;
		LoadedSceneManifest Manifest;
		CookedAssetFileSet Files;
		SceneAssetPayload Payload;
	};

	struct SceneLoadWorkState final
	{
		explicit SceneLoadWorkState(const std::filesystem::path& root) :
		    Publication(root)
		{
		}

		void PreparePackage(std::uint64_t requestId, std::uint64_t worldGeneration, std::uint64_t documentGeneration, LevelDesc level, const SceneAssetCatalog& catalog);
		void ReadAsset(std::size_t index);
		void DecodeAsset(std::size_t index);

	private:
		void ResolveAssets(const SceneAssetCatalog& catalog);

	public:
		Files::FileSetReadScope Publication;
		CookedAssetAllocationBudget AllocationBudget;
		std::unique_ptr<SceneLoadPackage> Package;
		std::vector<SceneAssetLoadWork> Assets;
		std::atomic<SceneLoadStatus> Status = SceneLoadStatus::Pending;
	};
}
