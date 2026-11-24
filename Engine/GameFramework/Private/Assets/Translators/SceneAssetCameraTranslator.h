#pragma once

#include "Scene/Camera/SceneCameraEntry.h"
#include "GameFramework/Public/Assets/Cooked/CookedSceneCameraRecord.h"

namespace Assets
{
	SceneCameraEntry BuildSceneAssetCamera(const CookedSceneCameraRecord& cameraRecord);
}
