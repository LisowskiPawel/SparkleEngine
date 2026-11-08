#pragma once

#include "Assets/Cooked/LoadedSceneManifest.h"
#include "Assets/SceneAssetPayload.h"

namespace Assets
{
	class CookedAssetFileSet;

	namespace SceneAssetPayloadMaterialAppender
	{
		void AppendMaterials(const LoadedSceneManifest& sceneManifest, CookedAssetFileSet& files, SceneAssetPayload& sceneAssetPayload);
	}
}
