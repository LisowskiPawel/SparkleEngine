#pragma once

#include "Assets/Payload/SceneAssetPayloadMeshBindings.h"

#include <span>

namespace Assets
{
	class CookedAssetAllocationBudget;

	namespace SceneAssetPayloadMaterialVariantAppender
	{
		void AppendMaterialVariants(
		    const LoadedSceneManifest& sceneManifest,
		    std::span<const SceneAssetPayloadMeshBinding> meshAssetBindings,
		    SceneAssetPayload& sceneAssetPayload,
		    CookedAssetAllocationBudget& budget);
	}
}
