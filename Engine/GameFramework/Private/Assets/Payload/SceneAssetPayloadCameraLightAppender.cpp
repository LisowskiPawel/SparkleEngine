#include "PCH.h"
#include "Assets/Loaders/CookedAssetAllocationBudget.h"

#include "Assets/Payload/SceneAssetPayloadCameraLightAppender.h"

#include "Assets/Translators/SceneAssetCameraTranslator.h"
#include "Assets/Translators/SceneAssetLightTranslator.h"

#include <cstddef>

namespace Assets
{
	static void AppendCameras(const LoadedSceneManifest& manifest, SceneAssetPayload& payload, CookedAssetAllocationBudget& budget)
	{
		budget.ReserveArray<SceneAssetPayload::Camera>(manifest.cameras.size());
		budget.Reserve(manifest.cameras.size(), sizeof(CookedSceneCameraRecord));
		payload.cameras.reserve(payload.cameras.size() + manifest.cameras.size());
		for (const auto& camera : manifest.cameras)
		{
			payload.cameras.push_back(BuildSceneAssetCamera(camera));
		}
	}

	static void AppendLights(const LoadedSceneManifest& manifest, SceneAssetPayload& payload, CookedAssetAllocationBudget& budget)
	{
		budget.ReserveArray<SceneLightDesc>(manifest.lights.size());
		budget.Reserve(manifest.lights.size(), sizeof(CookedSceneLightRecord));
		payload.lights.reserve(payload.lights.size() + manifest.lights.size());
		for (const auto& light : manifest.lights)
		{
			payload.lights.push_back(BuildSceneAssetLight(light));
		}
	}

	void SceneAssetPayloadCameraLightAppender::Append(const LoadedSceneManifest& manifest, SceneAssetPayload& payload, CookedAssetAllocationBudget& budget)
	{
		AppendCameras(manifest, payload, budget);
		AppendLights(manifest, payload, budget);
	}
}
