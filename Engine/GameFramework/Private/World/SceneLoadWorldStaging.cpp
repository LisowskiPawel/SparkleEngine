#include "PCH.h"
#include "World/SceneLoadWorldStaging.h"

#include "Core/Public/Diagnostics/Verify.h"
#include "Level/Loading/SceneLoadPackage.h"
#include "World/GameWorldSceneAssetCommitter.h"
#include "World/GameWorldState.h"
#include "World/Resources/GameWorldResourceStores.h"

#include <cstddef>
#include <optional>
#include <utility>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_sceneLoadStagingLogger, "GameFramework.GameWorld");

static void InitializeLevelCamera(ECS::GameWorldState& state, const LevelDesc& desc)
{
	if (state.GetCameraCount() == 0)
	{
		SceneCameraEntry entry;
		entry.name = "Scene Camera";
		entry.desc = desc.cameraDesc;
		if (!state.AddCamera(std::move(entry), true).IsValid())
		{
			Diagnostics::Fatal(g_sceneLoadStagingLogger, __FILE__, __LINE__, "The staged world rejected its level camera.");
		}
	}
	else
	{
		if (!state.WriteCameraDesc(state.GetActiveCamera(), desc.cameraDesc))
		{
			Diagnostics::Fatal(g_sceneLoadStagingLogger, __FILE__, __LINE__, "The staged world could not update its active camera.");
		}
	}
}

static void InitializeLevelSky(ECS::GameWorldState& state, const LevelDesc& desc)
{
	if (desc.sky)
	{
		state.WriteSkyEnvironment(*desc.sky);
	}
	else
	{
		state.RemoveSkyEnvironment();
	}
}

static void AppendLevelLights(ECS::GameWorldState& state, const LevelDesc& desc)
{
	for (const SceneLightDesc& light : desc.lights)
	{
		if (!state.AddLight(SceneLightDesc(light)).IsValid())
		{
			Diagnostics::Fatal(g_sceneLoadStagingLogger, __FILE__, __LINE__, "The staged world rejected a level light.");
		}
	}
}

static void SelectStagedCamera(ECS::GameWorldState& state)
{
	EntityId selectedCamera;
	for (std::size_t index = 1; index < state.GetCameraCount(); ++index)
	{
		const std::optional<SceneCameraEntry> camera = state.ReadCamera(state.GetCameraEntity(index));
		if (!camera)
		{
			Diagnostics::Fatal(g_sceneLoadStagingLogger, __FILE__, __LINE__, "The staged world could not read an enumerated camera.");
		}
		if (camera->IsPerspective())
		{
			selectedCamera = state.GetCameraEntity(index);
			break;
		}
	}
	if (!selectedCamera.IsValid() && state.GetCameraCount() != 0)
	{
		selectedCamera = state.GetCameraEntity(0);
	}
	if (selectedCamera.IsValid() && !state.SetActiveCamera(selectedCamera))
	{
		Diagnostics::Fatal(g_sceneLoadStagingLogger, __FILE__, __LINE__, "The staged world could not activate its selected camera.");
	}
}

void ValidateSceneLoadEntityCount(const Assets::SceneLoadPackage& package)
{
	std::size_t expectedEntityCount = 1 + package.Level.lights.size();
	for (const SceneAssetPayload& payload : package.AssetPayloads)
	{
		expectedEntityCount += payload.animations.size() + payload.staticMeshInstances.size() + payload.skeletalMeshInstances.size() + payload.cameras.size() + payload.lights.size();
	}
	if (package.Entities.size() != expectedEntityCount)
	{
		Diagnostics::Fatal(g_sceneLoadStagingLogger, __FILE__, __LINE__, "Scene load blueprint count does not match the entity construction records.");
	}
}

void InitializeSceneLoadLevel(ECS::GameWorldState& state, const LevelDesc& desc)
{
	InitializeLevelCamera(state, desc);
	InitializeLevelSky(state, desc);
	AppendLevelLights(state, desc);
}

void PopulateStagedScene(ECS::GameWorldState& state, GameWorldResourceStores& resources, Assets::SceneLoadPackage& package)
{
	for (SceneAssetPayload& payload : package.AssetPayloads)
	{
		CommitSceneAssetPayload(state, resources, std::move(payload));
	}
	SelectStagedCamera(state);
	if (!state.PrepareSystemResources(resources))
	{
		Diagnostics::Fatal(g_sceneLoadStagingLogger, __FILE__, __LINE__, "Staged world could not resolve animation targets and output slots.");
	}
	if (state.GetEntityCount() != package.Entities.size())
	{
		Diagnostics::Fatal(g_sceneLoadStagingLogger, __FILE__, __LINE__, "Staged world entity count does not match the validated blueprint package.");
	}
}
