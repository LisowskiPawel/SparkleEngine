#pragma once

#include "Assets/Payload/SceneAssetPayloadMeshBindings.h"
#include "Level/LevelDesc.h"

#include <cstdint>
#include <span>

namespace Assets
{
	class CookedAssetAllocationBudget;
	class CookedAssetFileSet;

	namespace SceneAssetPayloadMeshAppender
	{
		void AppendMeshAssets(const LoadedSceneManifest& sceneManifest, CookedAssetFileSet& files, SceneAssetPayload& sceneAssetPayload);

		void AppendMeshInstances(
		    const LoadedSceneManifest& sceneManifest,
		    std::span<const SceneAssetPayloadMeshBinding> meshAssetBindings,
		    SceneAssetPayload& sceneAssetPayload,
		    CookedAssetAllocationBudget& budget);

		void AppendMeshInstanceGroups(
		    const LoadedSceneManifest& sceneManifest,
		    std::span<const SceneAssetPayloadMeshBinding> meshAssetBindings,
		    SceneAssetPayload& sceneAssetPayload,
		    CookedAssetAllocationBudget& budget);
	}
}
