#pragma once

namespace Assets
{
	struct LoadedSceneManifest;

	namespace SceneManifestValidator
	{
		void ValidateHeader(const LoadedSceneManifest& manifest);
		void ValidateRecords(const LoadedSceneManifest& manifest);
	}
}
