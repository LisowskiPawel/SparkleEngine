#include "PCH.h"

#include "Scene/Commands/SceneObjectCommandFactory.h"

#include "Scene/Model/EditorSceneModel.h"
#include "Scene/SceneObjectSelection.h"

#include <utility>

std::optional<EditorCommandPair> SceneObjectCommandFactory::ToggleVisibility(const EditorSceneModel& model, const SceneObjectSelection& selection)
{
	const std::optional<bool> visibility = model.ReadVisibility(selection);
	if (!visibility)
	{
		return std::nullopt;
	}

	const bool bVisible = !*visibility;
	if (selection.type == SceneObjectType::Light)
	{
		const SceneLightDesc& before = model.FindLight(selection.entity)->Description;
		SceneLightDesc after = before;
		after.common.bVisible = bVisible;
		return EditorCommandPair{SetLightDescriptionCommand{selection.entity, std::move(after)}, SetLightDescriptionCommand{selection.entity, before}};
	}
	if (selection.type == SceneObjectType::Sky)
	{
		SkyEnvironment after = model.GetSkyEnvironment().value_or(SkyEnvironment{});
		after.bEnabled = bVisible;
		return EditorCommandPair{SetSkyEnvironmentCommand{after}, SetSkyEnvironmentCommand{model.GetSkyEnvironment()}};
	}
	if (selection.entity.IsValid())
	{
		return EditorCommandPair{SetEntityVisibilityCommand{selection.entity, bVisible}, SetEntityVisibilityCommand{selection.entity, *visibility}};
	}
	return std::nullopt;
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
	return EditorCommandPair{SetActiveCameraCommand{camera}, SetActiveCameraCommand{previous}};
}
