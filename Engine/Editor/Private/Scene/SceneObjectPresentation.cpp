#include "PCH.h"
#include "Icons/EditorIcon.h"
#include "Scene/SceneObjectPresentation.h"

#include "Scene/Model/EditorSceneModel.h"
#include "Scene/SceneObjectSelection.h"
#include "Scene/Lighting/SceneLightDesc.h"
#include "Style/SparkleUiPalette.h"

#include <imgui.h>

namespace SceneObjectPresentation
{
	void DrawSelectionHeader(const EditorSceneModel* model, const SceneObjectSelection* selection) noexcept
	{
		const EditorSceneEntry* entry = model && selection ? model->FindEntry(*selection) : nullptr;
		const char* title = entry != nullptr ? entry->Label.c_str() : "No Selection";
		const char* subtitle = entry != nullptr ? entry->TypeLabel.c_str() : "Object";
		const EditorIcons::EditorIcon icon = SceneObjectPresentation::BuildSelectionIcon(selection, entry ? entry->LightKind : SceneLightKind::Unknown);

		constexpr float kHeaderHeight = 30.0f;
		constexpr float kHeaderPaddingX = 8.0f;
		const float width = ImGui::GetContentRegionAvail().x;
		const ImVec2 start = ImGui::GetCursorScreenPos();
		const ImVec2 end(start.x + width, start.y + kHeaderHeight);
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		drawList->AddRectFilled(start, end, ImGui::ColorConvertFloat4ToU32(SparkleUiPalette::HeaderBackgroundActive()));
		drawList->AddRectFilled(start, ImVec2(start.x + 3.0f, end.y), ImGui::ColorConvertFloat4ToU32(SparkleUiPalette::Accent()));
		drawList->AddLine(ImVec2(start.x, start.y), ImVec2(end.x, start.y), SparkleUiPalette::WithAlphaU32(SparkleUiPalette::Border(), 0.52f));
		drawList->AddLine(ImVec2(start.x, end.y - 1.0f), ImVec2(end.x, end.y - 1.0f), SparkleUiPalette::WithAlphaU32(SparkleUiPalette::Border(), 0.58f));

		ImGui::InvisibleButton("##DetailsSelectionHeader", ImVec2(width, kHeaderHeight));

		ImGui::SetCursorScreenPos(ImVec2(start.x + kHeaderPaddingX + 10.0f, start.y + 7.0f));
		ImGui::TextUnformatted(EditorIcons::GetEditorIconGlyph(icon));
		ImGui::SameLine(0.0f, 8.0f);
		ImGui::TextUnformatted(title);
		ImGui::SameLine();
		ImGui::TextDisabled("%s", subtitle);

		ImGui::SetCursorScreenPos(ImVec2(start.x, end.y));
	}

	EditorIcons::EditorIcon GetLightIcon(SceneLightKind kind) noexcept
	{
		switch (kind)
		{
			case SceneLightKind::Directional:
				return EditorIcons::EditorIcon::DirectionalLight;
			case SceneLightKind::Point:
				return EditorIcons::EditorIcon::PointLight;
			case SceneLightKind::Spot:
				return EditorIcons::EditorIcon::SpotLight;
			case SceneLightKind::Rect:
				return EditorIcons::EditorIcon::Light;
			case SceneLightKind::Unknown:
			default:
				return EditorIcons::EditorIcon::Light;
		}
	}

	const char* GetLightTypeLabel(SceneLightKind kind) noexcept
	{
		switch (kind)
		{
			case SceneLightKind::Directional:
				return "Directional Light";
			case SceneLightKind::Point:
				return "Point Light";
			case SceneLightKind::Spot:
				return "Spot Light";
			case SceneLightKind::Rect:
				return "Rect Light";
			case SceneLightKind::Unknown:
			default:
				return "Light";
		}
	}

	std::string BuildLightLabel(const SceneLightDesc& light, std::size_t lightIndex)
	{
		if (!light.common.name.empty())
		{
			return light.common.name;
		}

		return std::string(GetLightTypeLabel(light.GetKind())) + " " + std::to_string(lightIndex + 1);
	}

	EditorIcons::EditorIcon BuildSelectionIcon(const SceneObjectSelection& selection, SceneLightKind lightKind) noexcept
	{
		switch (selection.type)
		{
			case SceneObjectType::Camera:
				return EditorIcons::EditorIcon::Camera;
			case SceneObjectType::Sky:
				return EditorIcons::EditorIcon::ViewLit;
			case SceneObjectType::Light:
				return GetLightIcon(lightKind);
			case SceneObjectType::Mesh:
				return EditorIcons::EditorIcon::StaticMesh;
			case SceneObjectType::None:
			default:
				return EditorIcons::EditorIcon::None;
		}
	}

	EditorIcons::EditorIcon BuildSelectionIcon(const SceneObjectSelection* selection, SceneLightKind lightKind) noexcept
	{
		return selection != nullptr ? BuildSelectionIcon(*selection, lightKind) : EditorIcons::EditorIcon::None;
	}
}
