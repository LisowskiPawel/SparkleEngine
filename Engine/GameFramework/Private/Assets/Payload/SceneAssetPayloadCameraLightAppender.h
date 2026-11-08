#pragma once

#include "Assets/Cooked/LoadedSceneManifest.h"
#include "Assets/SceneAssetPayload.h"

namespace Assets
{
	class CookedAssetAllocationBudget;

	namespace SceneAssetPayloadCameraLightAppender
	{
		void Append(const LoadedSceneManifest& sceneManifest, SceneAssetPayload& sceneAssetPayload, CookedAssetAllocationBudget& budget);
	}
}
