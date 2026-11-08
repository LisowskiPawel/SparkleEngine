#pragma once

#include "Assets/Cooked/LoadedSceneManifest.h"
#include "Assets/SceneAssetPayload.h"

#include <vector>

namespace Assets
{
	class CookedAssetAllocationBudget;

	struct SceneAssetPayloadMeshBinding final
	{
		CookedMeshAssetKind kind = CookedMeshAssetKind::Static;
		SceneMeshAssetIndex payloadMeshAssetIndex = kInvalidSceneMeshAssetIndex;
	};

	std::vector<SceneAssetPayloadMeshBinding> BuildSceneAssetPayloadMeshBindings(const LoadedSceneManifest& sceneManifest, CookedAssetAllocationBudget& budget);
}
