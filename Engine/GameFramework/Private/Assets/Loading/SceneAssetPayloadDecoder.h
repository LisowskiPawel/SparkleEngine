#pragma once

#include "Assets/SceneAssetPayload.h"

namespace Assets
{
	struct LoadedSceneManifest;
	class CookedAssetFileSet;

	namespace SceneAssetPayloadDecoder
	{
		SceneAssetPayload Decode(const LoadedSceneManifest& manifest, CookedAssetFileSet& files);
	}
}
