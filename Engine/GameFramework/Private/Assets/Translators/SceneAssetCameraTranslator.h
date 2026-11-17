#pragma once

#include "Assets/SceneAssetPayload.h"
#include "Assets/Cooked/LoadedSceneManifest.h"
#include "GameFramework/Public/Assets/Cooked/CookedSceneCameraRecord.h"

namespace Assets
{
	SceneAssetPayload::Camera BuildSceneAssetCamera(const CookedSceneCameraRecord& cameraRecord);
}
