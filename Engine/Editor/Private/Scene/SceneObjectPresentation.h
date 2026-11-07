#pragma once

#include "Scene/Lighting/SceneLightKind.h"
#include "Icons/EditorIcon.h"

#include <cstddef>
#include <string>

struct SceneLightDesc;
struct SceneObjectSelection;
class EditorSceneModel;

namespace SceneObjectPresentation
{
	void DrawSelectionHeader(const EditorSceneModel* model, const SceneObjectSelection* selection) noexcept;
	EditorIcons::EditorIcon GetLightIcon(SceneLightKind kind) noexcept;
	const char* GetLightTypeLabel(SceneLightKind kind) noexcept;
	std::string BuildLightLabel(const SceneLightDesc& light, std::size_t lightIndex);
	EditorIcons::EditorIcon BuildSelectionIcon(const SceneObjectSelection& selection, SceneLightKind lightKind = SceneLightKind::Unknown) noexcept;
	EditorIcons::EditorIcon BuildSelectionIcon(const SceneObjectSelection* selection, SceneLightKind lightKind = SceneLightKind::Unknown) noexcept;
}
