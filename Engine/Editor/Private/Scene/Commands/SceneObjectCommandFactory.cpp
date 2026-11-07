#include "PCH.h"

#include "SceneObjectCommandFactory.h"

#include "Scene/Model/EditorSceneModel.h"
#include "Scene/SceneObjectSelection.h"

std::optional<EditorCommandPair> SceneObjectCommandFactory::SetVisibility(const EditorSceneModel& model, const SceneObjectSelection& selection, bool bVisible)
{
	EditorCommandPair commands;
	if (selection.type == SceneObjectType::Light)
	{
		const WorldLightReadData* light = model.FindLight(selection.entity);
		if (light == nullptr)
		{
			return std::nullopt;
		}
		SceneLightDesc after = light->Description;
		after.common.bVisible = bVisible;
		commands.Forward.Payload = SetLightDescriptionCommand{selection.entity, after};
		commands.Inverse.Payload = SetLightDescriptionCommand{selection.entity, light->Description};
	}
	else if (selection.type == SceneObjectType::Sky)
	{
		SkyEnvironment after = model.GetSkyEnvironment().value_or(SkyEnvironment{});
		after.bEnabled = bVisible;
		commands.Forward.Payload = SetSkyEnvironmentCommand{after};
		commands.Inverse.Payload = SetSkyEnvironmentCommand{model.GetSkyEnvironment()};
	}
	else if (selection.entity.IsValid())
	{
		const EditorSceneEntry* entry = model.FindEntry(selection);
		if (entry == nullptr)
		{
			return std::nullopt;
		}
		commands.Forward.Payload = SetEntityVisibilityCommand{selection.entity, bVisible};
		commands.Inverse.Payload = SetEntityVisibilityCommand{selection.entity, entry->bVisible};
	}
	else
	{
		return std::nullopt;
	}
	return commands;
}

std::optional<EditorCommandPair> SceneObjectCommandFactory::SetActiveCamera(const EditorSceneModel& model, EntityId camera)
{
	if (model.FindCamera(camera) == nullptr)
	{
		return std::nullopt;
	}
	EntityId previous = EntityId::Invalid();
	for (const WorldCameraReadData& entry : model.GetCameras())
	{
		if (entry.bActive)
		{
			previous = entry.Entity;
		}
	}
	if (previous == camera || !previous.IsValid())
	{
		return std::nullopt;
	}

	EditorCommandPair commands;
	commands.Forward.Payload = SetActiveCameraCommand{camera};
	commands.Inverse.Payload = SetActiveCameraCommand{previous};
	return commands;
}
