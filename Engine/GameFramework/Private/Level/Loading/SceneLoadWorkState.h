#pragma once

#include "Level/Loading/SceneLoadExecutor.h"
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

		Files::FileSetReadScope Publication;
		CookedAssetAllocationBudget AllocationBudget;
		std::unique_ptr<SceneLoadPackage> Package;
		std::vector<SceneAssetLoadWork> Assets;
		std::atomic<std::uint32_t> CompletedDecodes = 0;
		std::atomic<LevelLoadOperationStage> Stage = LevelLoadOperationStage::Reading;
	};
}
