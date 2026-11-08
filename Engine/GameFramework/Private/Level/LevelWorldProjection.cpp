#include "PCH.h"
#include "Level/LevelWorldProjection.h"
#include "World/WorldReadView.h"

LevelDesc CaptureLevelWorldState(LevelDesc description, const WorldReadView& view)
{
	description.lights.clear();
	description.lights.reserve(view.GetLights().size());
	for (const WorldLightReadData& light : view.GetLights())
	{
		description.lights.push_back(light.Description);
	}
	description.sky = view.GetSkyEnvironment();
	for (const WorldCameraReadData& camera : view.GetCameras())
	{
		if (camera.bActive)
		{
			description.cameraDesc = camera.Description;
			break;
		}
	}
	return description;
}
