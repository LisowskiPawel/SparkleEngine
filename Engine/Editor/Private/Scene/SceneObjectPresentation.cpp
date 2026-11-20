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
	void DrawSelectionHeader(const EditorSceneModel& model, const SceneObjectSelection& selection) noexcept
	{
		const EditorSceneEntry* entry = model.FindEntry(selection);
		const char* title = entry != nullptr ? entry->Label.c_str() : "No Selection";
		const char* subtitle = entry != nullptr ? entry->TypeLabel.c_str() : "Object";
		const EditorIcons::EditorIcon icon = SceneObjectPresentation::BuildSelectionIcon(selection, entry ? entry->LightKind : SceneLightKind::Unknown);

		constexpr float kHeaderHeight = 30.0f;
		constexpr float kHeaderPaddingX = 8.0f;
		const float width = ImGui::GetContentRegionAvail().x;
		const ImVec2 start = ImGui::GetCursorScreenPos();
		const ImVec2 end(start.x + width, start.y + kHeaderHeight);
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		const ImU32 backgroundColor = ImGui::ColorConvertFloat4ToU32(SparkleUiPalette::HeaderBackgroundActive());
		drawList->AddRectFilled(start, end, backgroundColor);
		const ImVec2 accentEnd(start.x + 3.0f, end.y);
		const ImU32 accentColor = ImGui::ColorConvertFloat4ToU32(SparkleUiPalette::Accent());
		drawList->AddRectFilled(start, accentEnd, accentColor);
		const ImVec2 topBorderEnd(end.x, start.y);
		const ImU32 topBorderColor = SparkleUiPalette::WithAlphaU32(SparkleUiPalette::Border(), 0.52f);
		drawList->AddLine(start, topBorderEnd, topBorderColor);
		const ImVec2 bottomBorderStart(start.x, end.y - 1.0f);
		const ImVec2 bottomBorderEnd(end.x, end.y - 1.0f);
		const ImU32 bottomBorderColor = SparkleUiPalette::WithAlphaU32(SparkleUiPalette::Border(), 0.58f);
		drawList->AddLine(bottomBorderStart, bottomBorderEnd, bottomBorderColor);

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
}
