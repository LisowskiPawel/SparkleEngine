#include "PCH.h"
#include "Scene/Inspection/SceneInspectorPanel.h"

#include "Scene/Inspection/SceneCameraInspector.h"
#include "Scene/Inspection/SceneLightInspector.h"
#include "Scene/Inspection/SceneMaterialVariantInspector.h"
#include "Scene/Inspection/SceneMeshInspector.h"
#include "Scene/Inspection/SceneSkyInspector.h"
#include "Scene/EditorSceneSession.h"
#include "Scene/SceneObjectPresentation.h"
#include "Style/SparkleUiPalette.h"
#include "Icons/EditorIcon.h"
#include "Widgets/UiDetailsWidgets.h"

#include <algorithm>
#include <string>
#include <optional>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

SceneInspectorPanel::SceneInspectorPanel(EditorSceneSession& scene, float widthPixels) noexcept :
    m_scene(scene),
    m_widthPixels(widthPixels)
{
}

void SceneInspectorPanel::BuildDetailsToolbar() noexcept
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 5.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.055f, 0.058f, 0.064f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.075f, 0.080f, 0.090f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.070f, 0.095f, 0.130f, 1.0f));
	const ImU32 borderColor = SparkleUiPalette::WithAlphaU32(SparkleUiPalette::Border(), 0.70f);
	ImGui::PushStyleColor(ImGuiCol_Border, borderColor);
	ImGui::SetNextItemWidth(-1.0f);
	const std::string searchHint = EditorIcons::MakeIconLabel(EditorIcons::EditorIcon::Search, "Search");
	ImGui::InputTextWithHint("##DetailsFilter", searchHint.c_str(), &m_filterText);
	ImGui::PopStyleColor(4);
	ImGui::PopStyleVar(3);
}

void SceneInspectorPanel::SetTopInset(float topInsetPixels) noexcept
{
	m_topInsetPixels = topInsetPixels;
}

void SceneInspectorPanel::DrawSelectionHeader() noexcept
{
	const SceneObjectSelection& selection = m_scene.GetSelection();
	const auto entry = SceneObjectPresentation::BuildEntry(m_scene.GetModel(), selection);
	const char* title = entry ? entry->GetLabel().data() : "No Selection";
	const char* subtitle = entry ? entry->TypeLabel : "Object";
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

void SceneInspectorPanel::BuildSelectionInspector() noexcept
{
	const EditorSceneModel& model = m_scene.GetModel();
	const SceneObjectSelection& selection = m_scene.GetSelection();
	EditorTransactionHistory& history = m_scene.GetHistory();
	DrawSelectionHeader();
	BuildDetailsToolbar();
	switch (selection.type)
	{
		case SceneObjectType::Camera:
			if (const WorldCameraReadData* camera = model.FindCamera(selection.entity))
			{
				SceneCameraInspector::Build(*camera, history, model.GetWorldGeneration(), m_filterText);
			}
			break;
		case SceneObjectType::Light:
			if (const WorldLightReadData* light = model.FindLight(selection.entity))
			{
				SceneLightInspector::Build(light->Description, light->Entity, history, model.GetWorldGeneration(), m_filterText);
			}
			break;
		case SceneObjectType::Sky:
			SceneSkyInspector::Build(model.GetSkyEnvironment(), history, model.GetWorldGeneration(), m_filterText);
			break;
		case SceneObjectType::Mesh:
			if (const WorldMeshReadData* mesh = model.FindMesh(selection.entity))
			{
				SceneMeshInspector::Build(*mesh, history, model.GetWorldGeneration(), m_filterText);
			}
			break;
		case SceneObjectType::None:
		default:
			EditorWidgets::DrawDetailsEmptyState();
			break;
	}
}

void SceneInspectorPanel::BuildEditStatus() noexcept
{
	EditorTransactionHistory& history = m_scene.GetHistory();
	const std::optional<WorldEditResult>& result = history.GetLastResult();
	if (!result || result->IsAccepted())
	{
		return;
	}

	ImGui::TextWrapped("Edit not applied: %s", result->Message.c_str());
	if (ImGui::SmallButton("Dismiss edit failure"))
	{
		history.DismissLastResult();
	}
	ImGui::Separator();
}

void SceneInspectorPanel::BeginWindow() noexcept
{
	ImGuiIO& io = ImGui::GetIO();

	constexpr float kMinWidth = 320.0f;
	const float kMaxWidth = (std::max) (kMinWidth + 1.0f, io.DisplaySize.x * 0.7f);
	m_widthPixels = std::clamp(m_widthPixels, kMinWidth, kMaxWidth);
	const float panelHeight = (std::max) (1.0f, io.DisplaySize.y - m_topInsetPixels);

	ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - m_widthPixels, m_topInsetPixels), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(m_widthPixels, panelHeight), ImGuiCond_Once);
	ImGui::SetNextWindowSizeConstraints(ImVec2(kMinWidth, panelHeight), ImVec2(kMaxWidth, panelHeight));

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
	ImGui::PopStyleVar();

	m_widthPixels = ImGui::GetWindowWidth();
}

void SceneInspectorPanel::BuildInspectorTabs() noexcept
{
	const EditorSceneModel& model = m_scene.GetModel();
	constexpr ImGuiTabBarFlags kTabBarFlags = ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_FittingPolicyResizeDown;
	if (ImGui::BeginTabBar("##InspectorTabs", kTabBarFlags))
	{
		if (ImGui::BeginTabItem("Details"))
		{
			ImGui::Spacing();
			BuildSelectionInspector();
			ImGui::EndTabItem();
		}

		if (!model.GetMaterialVariants().Names.empty() && ImGui::BeginTabItem("Variants"))
		{
			ImGui::Spacing();
			SceneMaterialVariantInspector::Build(model.GetMaterialVariants(), m_scene.GetHistory(), model.GetWorldGeneration());
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void SceneInspectorPanel::BuildUI(bool bDisableInteraction)
{
	BeginWindow();

	if (!m_scene.GetModel().HasWorld())
	{
		ImGui::TextDisabled("Scene inspector unavailable");
		ImGui::End();
		return;
	}

	ImGui::BeginDisabled(bDisableInteraction);

	constexpr float kContentPad = 8.0f;
	ImGui::Indent(kContentPad);
	BuildEditStatus();
	BuildInspectorTabs();

	ImGui::Unindent(kContentPad);

	ImGui::EndDisabled();

	ImGui::End();
}
