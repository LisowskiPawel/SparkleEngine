#pragma once

#include "Level/Loading/SceneLoadExecutor.h"
#include "Assets/Cooked/LoadedSceneManifest.h"
#include "Assets/Loaders/CookedAssetFileSet.h"
#include "Core/Public/Files/FileSetPublication.h"
#include "Tasks/Public/TaskTypes.h"

#include <cstdint>
#include <atomic>
#include <filesystem>
#include <memory>
#include <vector>

class TaskExecutionContext;

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
		TaskResult ReadAsset(std::size_t index, TaskExecutionContext& context);
		TaskResult DecodeAsset(std::size_t index, TaskExecutionContext& context);
		TaskResult FinalizePackage(TaskExecutionContext& context);

	private:
		void ResolveAssets(const SceneAssetCatalog& catalog);

	public:
		Files::FileSetReadScope Publication;
		CookedAssetAllocationBudget AllocationBudget;
		std::unique_ptr<SceneLoadPackage> Package;
		std::vector<SceneAssetLoadWork> Assets;
		std::atomic<std::uint32_t> CompletedDecodes = 0;
		std::atomic<LevelLoadOperationStage> Stage = LevelLoadOperationStage::Reading;
	};
}
