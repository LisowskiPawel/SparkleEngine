#pragma once

struct SceneAssetPayload;

namespace Assets
{
	struct LoadedSceneManifest;
	class CookedAssetFileSet;

	void DecodeSceneMeshes(const LoadedSceneManifest& manifest, CookedAssetFileSet& files, SceneAssetPayload& payload);
}
