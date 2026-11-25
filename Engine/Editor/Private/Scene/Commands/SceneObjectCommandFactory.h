#pragma once

#include "World/WorldEditCommand.h"

#include <optional>

class EditorSceneModel;
struct SceneObjectSelection;

struct EditorCommandPair final
{
	WorldEditPayload Forward;
	WorldEditPayload Inverse;
};

namespace SceneObjectCommandFactory
{
	std::optional<EditorCommandPair> ToggleVisibility(const EditorSceneModel& model, const SceneObjectSelection& selection);
	std::optional<EditorCommandPair> SetActiveCamera(const EditorSceneModel& model, EntityId camera);
}
