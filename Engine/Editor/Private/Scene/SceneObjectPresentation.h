#pragma once

#include "Scene/Lighting/SceneLightKind.h"
#include "Icons/EditorIcon.h"
#include "Scene/SceneObjectSelection.h"
#include "World/SkyEnvironment.h"

#include <cstddef>
#include <string>
#include <optional>
#include <string_view>
#include <variant>

class EditorSceneModel;
struct WorldCameraReadData;
struct WorldLightReadData;
struct WorldMeshReadData;

namespace SceneObjectPresentation
{
	struct Entry final
	{
		// Authored names borrow the retained world view; generated labels own their text.
		std::variant<std::string_view, std::string> Label;
		const char* TypeLabel;
		SceneObjectSelection Selection;
		bool bVisible;
		SceneLightKind LightKind = SceneLightKind::Unknown;

		std::string_view GetLabel() const noexcept;
	};

	Entry BuildEntry(const WorldCameraReadData& camera, std::size_t index);
	Entry BuildEntry(const WorldLightReadData& light, std::size_t index);
	Entry BuildEntry(const WorldMeshReadData& mesh, std::size_t index);
	Entry BuildEntry(const std::optional<SkyEnvironment>& sky, std::size_t index);
	std::optional<Entry> BuildEntry(const EditorSceneModel& scene, const SceneObjectSelection& selection);
	void DrawSelectionHeader(const EditorSceneModel& scene, const SceneObjectSelection& selection) noexcept;
	EditorIcons::EditorIcon GetLightIcon(SceneLightKind kind) noexcept;
	const char* GetLightTypeLabel(SceneLightKind kind) noexcept;
	EditorIcons::EditorIcon BuildSelectionIcon(const SceneObjectSelection& selection, SceneLightKind lightKind = SceneLightKind::Unknown) noexcept;
}
