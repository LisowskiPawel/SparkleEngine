#pragma once

#include "Assets/Cooked/CookedSceneLightRecord.h"
#include "Scene/Lighting/SceneLightDesc.h"

namespace Assets
{
	SceneLightDesc BuildSceneAssetLight(const CookedSceneLightRecord& lightRecord);
}
