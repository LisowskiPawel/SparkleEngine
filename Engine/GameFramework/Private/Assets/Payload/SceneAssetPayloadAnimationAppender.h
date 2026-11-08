#pragma once

#include "Assets/Cooked/LoadedSceneManifest.h"
#include "Assets/SceneAssetPayload.h"

namespace Assets
{
	class CookedAssetFileSet;

	namespace SceneAssetPayloadAnimationAppender
	{
		void AppendAnimations(const LoadedSceneManifest& sceneManifest, CookedAssetFileSet& files, SceneAssetPayload& sceneAssetPayload);
	}
}
