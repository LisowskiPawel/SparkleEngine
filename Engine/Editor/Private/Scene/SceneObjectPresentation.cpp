#include "PCH.h"
#include "Scene/SceneObjectPresentation.h"

#include "Scene/Model/EditorSceneModel.h"
#include "Scene/SceneObjectSelection.h"
#include "Scene/Lighting/SceneLightDesc.h"
#include "Style/SparkleUiPalette.h"

#include <imgui.h>

namespace SceneObjectPresentation
{
	static std::string BuildSelectionTitle(const EditorSceneModel* model, const SceneObjectSelection* selection)
	{
		if (selection == nullptr)
		{
			return "No Selection";
		}

		switch (selection->type)
		{
			case SceneObjectType::Camera:
				if (model)
				{
					const WorldCameraReadData* camera = model->FindCamera(selection->entity);
					return camera && !camera->Name.empty() ? camera->Name : "Camera";
				}
				return "Scene Camera";
			case SceneObjectType::Light:
				if (model)
				{
					if (const WorldLightReadData* light = model->FindLight(selection->entity))
					{
						return SceneObjectPresentation::BuildLightLabel(light->Description, selection->entity.GetSlot());
					}
				}
				return "Light";
			case SceneObjectType::Sky:
				return "Sky";
			case SceneObjectType::Mesh:
				return "Mesh " + std::to_string(selection->entity.GetSlot() + 1u);
			case SceneObjectType::None:
			default:
				return "No Selection";
		}
	}

	static const char* BuildSelectionSubtitle(const EditorSceneModel* model, const SceneObjectSelection* selection) noexcept
	{
		if (selection == nullptr)
		{
			return "Object";
		}

		switch (selection->type)
		{
			case SceneObjectType::Camera:
				return "Camera";
			case SceneObjectType::Light:
				if (model)
				{
					if (const WorldLightReadData* light = model->FindLight(selection->entity))
					{
						return SceneObjectPresentation::GetLightTypeLabel(light->Description.GetKind());
					}
				}
				return "Light";
			case SceneObjectType::Sky:
				return "Sky";
			case SceneObjectType::Mesh:
				return "Static Mesh";
			case SceneObjectType::None:
			default:
				return "Object";
		}
	}

	void DrawSelectionHeader(const EditorSceneModel* model, const SceneObjectSelection* selection) noexcept
	{
		const std::string title = BuildSelectionTitle(model, selection);
		const char* subtitle = BuildSelectionSubtitle(model, selection);
		const EditorSceneEntry* entry = model && selection ? model->FindEntry(*selection) : nullptr;
		const UiUtil::EditorIcon icon = SceneObjectPresentation::BuildSelectionIcon(selection, entry ? entry->LightKind : SceneLightKind::Unknown);

		constexpr float kHeaderHeight = 30.0f;
		constexpr float kHeaderPaddingX = 8.0f;
		const float width = ImGui::GetContentRegionAvail().x;
		const ImVec2 start = ImGui::GetCursorScreenPos();
		const ImVec2 end(start.x + width, start.y + kHeaderHeight);
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		drawList->AddRectFilled(start, end, ImGui::ColorConvertFloat4ToU32(SparkleUiPalette::HeaderBackgroundActive()));
		drawList->AddRectFilled(start, ImVec2(start.x + 3.0f, end.y), ImGui::ColorConvertFloat4ToU32(SparkleUiPalette::Accent()));
		drawList->AddLine(ImVec2(start.x, start.y), ImVec2(end.x, start.y), UiUtil::WithAlphaU32(SparkleUiPalette::Border(), 0.52f));
		drawList->AddLine(ImVec2(start.x, end.y - 1.0f), ImVec2(end.x, end.y - 1.0f), UiUtil::WithAlphaU32(SparkleUiPalette::Border(), 0.58f));

		ImGui::InvisibleButton("##DetailsSelectionHeader", ImVec2(width, kHeaderHeight));

		ImGui::SetCursorScreenPos(ImVec2(start.x + kHeaderPaddingX + 10.0f, start.y + 7.0f));
		ImGui::TextUnformatted(UiUtil::GetEditorIconGlyph(icon));
		ImGui::SameLine(0.0f, 8.0f);
		ImGui::TextUnformatted(title.c_str());
		ImGui::SameLine();
		ImGui::TextDisabled("%s", subtitle);

		ImGui::SetCursorScreenPos(ImVec2(start.x, end.y));
	}

	UiUtil::EditorIcon GetLightIcon(SceneLightKind kind) noexcept
	{
		switch (kind)
		{
			case SceneLightKind::Directional:
				return UiUtil::EditorIcon::DirectionalLight;
			case SceneLightKind::Point:
				return UiUtil::EditorIcon::PointLight;
			case SceneLightKind::Spot:
				return UiUtil::EditorIcon::SpotLight;
			case SceneLightKind::Rect:
				return UiUtil::EditorIcon::Light;
			case SceneLightKind::Unknown:
			default:
				return UiUtil::EditorIcon::Light;
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

	UiUtil::EditorIcon BuildSelectionIcon(const SceneObjectSelection& selection, SceneLightKind lightKind) noexcept
	{
		switch (selection.type)
		{
			case SceneObjectType::Camera:
				return UiUtil::EditorIcon::Camera;
			case SceneObjectType::Sky:
				return UiUtil::EditorIcon::ViewLit;
			case SceneObjectType::Light:
				return GetLightIcon(lightKind);
			case SceneObjectType::Mesh:
				return UiUtil::EditorIcon::StaticMesh;
			case SceneObjectType::None:
			default:
				return UiUtil::EditorIcon::None;
		}
	}

	UiUtil::EditorIcon BuildSelectionIcon(const SceneObjectSelection* selection, SceneLightKind lightKind) noexcept
	{
		return selection != nullptr ? BuildSelectionIcon(*selection, lightKind) : UiUtil::EditorIcon::None;
	}
}
