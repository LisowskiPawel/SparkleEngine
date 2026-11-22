#include "PCH.h"
#include "Scene/SceneOutlinerPanel.h"

#include "Core/Public/Strings/StringUtils.h"
#include "Scene/EditorSceneSession.h"
#include "Scene/SceneObjectPresentation.h"
#include "Style/SparkleUiPalette.h"
#include "Icons/EditorIcon.h"
#include "Widgets/UiIconWidgets.h"
#include "Widgets/UiPanelWidgets.h"

#include <algorithm>
#include <string>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

SceneOutlinerPanel::SceneOutlinerPanel(EditorSceneSession& scene, float widthPixels) noexcept :
    m_scene(scene),
    m_widthPixels(widthPixels)
{
}

void SceneOutlinerPanel::SetTopInset(float topInsetPixels) noexcept
{
	m_topInsetPixels = topInsetPixels;
}

void SceneOutlinerPanel::BuildToolbar() noexcept
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 3.0f));
	ImGui::SetNextItemWidth(-1.0f);
	const std::string searchHint = EditorIcons::MakeIconLabel(EditorIcons::EditorIcon::Search, "Search actors, lights, meshes...");
	ImGui::InputTextWithHint("##SceneFilter", searchHint.c_str(), &m_filterText);

	ImGui::Spacing();
	if (EditorWidgets::DrawFilterChip("All", m_activeFilter == SceneOutlinerFilter::All))
	{
		m_activeFilter = SceneOutlinerFilter::All;
	}
	ImGui::SameLine(0.0f, 4.0f);
	if (EditorWidgets::DrawFilterChip("Camera", m_activeFilter == SceneOutlinerFilter::Cameras))
	{
		m_activeFilter = SceneOutlinerFilter::Cameras;
	}
	ImGui::SameLine(0.0f, 4.0f);
	if (EditorWidgets::DrawFilterChip("Light", m_activeFilter == SceneOutlinerFilter::Lights))
	{
		m_activeFilter = SceneOutlinerFilter::Lights;
	}
	ImGui::SameLine(0.0f, 4.0f);
	if (EditorWidgets::DrawFilterChip("Mesh", m_activeFilter == SceneOutlinerFilter::Meshes))
	{
		m_activeFilter = SceneOutlinerFilter::Meshes;
	}
	ImGui::PopStyleVar();
}

void SceneOutlinerPanel::BuildFooter() noexcept
{
	const EditorSceneModel& model = *m_scene.GetModel();
	const std::size_t totalCount = 1u + model.GetCameras().size() + model.GetLights().size() + model.GetMeshes().size();
	const std::size_t displayedCount = CountVisibleEntries();
	const bool bHasValidSelection = model.Contains(m_scene.GetSelection());
	ImGui::Separator();
	ImGui::TextDisabled("%zu actors%s", displayedCount, bHasValidSelection ? " (1 selected)" : "");
	if (displayedCount != totalCount)
	{
		ImGui::SameLine();
		ImGui::TextDisabled("of %zu", totalCount);
	}
}

void SceneOutlinerPanel::DrawEntrySection(SceneOutlinerFilter filter, const char* id, const char* label, const char* emptyText, std::span<const EditorSceneEntry> entries) noexcept
{
	if (m_activeFilter != SceneOutlinerFilter::All && m_activeFilter != filter)
	{
		return;
	}

	bool bOpen = true;
	DrawSectionRow(id, label, entries.size(), bOpen);
	if (bOpen)
	{
		if (entries.empty())
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(1);
			ImGui::TextDisabled("%s", emptyText);
		}

		for (const EditorSceneEntry& entry : entries)
		{
			if (MatchesFilter(entry))
			{
				DrawSelectionEntry(entry.Label.c_str(), entry.TypeLabel.c_str(), entry.Selection);
			}
		}
	}
}

bool SceneOutlinerPanel::PassesActiveFilter(const SceneObjectSelection& selection) const noexcept
{
	switch (m_activeFilter)
	{
		case SceneOutlinerFilter::Cameras:
			return selection.type == SceneObjectType::Camera;
		case SceneOutlinerFilter::Lights:
			return selection.type == SceneObjectType::Light;
		case SceneOutlinerFilter::Meshes:
			return selection.type == SceneObjectType::Mesh;
		case SceneOutlinerFilter::All:
		default:
			return true;
	}
}

bool SceneOutlinerPanel::MatchesFilter(const EditorSceneEntry& entry) const noexcept
{
	if (!PassesActiveFilter(entry.Selection))
	{
		return false;
	}
	if (m_filterText.empty())
	{
		return true;
	}
	const bool bLabelMatches = Strings::ContainsIgnoreCase(entry.Label.c_str(), m_filterText);
	return bLabelMatches || Strings::ContainsIgnoreCase(entry.TypeLabel.c_str(), m_filterText);
}

std::size_t SceneOutlinerPanel::CountVisibleEntries() const noexcept
{
	std::size_t count = 0;

	const auto countVisible = [this](std::span<const EditorSceneEntry> entries) noexcept
	{
		std::size_t visibleCount = 0;
		for (const EditorSceneEntry& entry : entries)
		{
			if (MatchesFilter(entry))
			{
				++visibleCount;
			}
		}
		return visibleCount;
	};

	const EditorSceneModel& model = *m_scene.GetModel();
	count += countVisible(model.GetCameraEntries());
	const EditorSceneEntry& sky = model.GetSkyEntry();
	if (MatchesFilter(sky))
	{
		++count;
	}
	count += countVisible(model.GetLightEntries());
	count += countVisible(model.GetMeshEntries());

	return count;
}

bool SceneOutlinerPanel::IsEntryVisible(const SceneObjectSelection& selection) const noexcept
{
	const EditorSceneEntry* entry = m_scene.GetModel()->FindEntry(selection);
	return entry == nullptr || entry->bVisible;
}

void SceneOutlinerPanel::DrawSectionRow(const char* id, const char* label, std::size_t count, bool& bOpen) noexcept
{
	ImGui::TableNextRow();
	ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, SparkleUiPalette::SectionHeaderBackground());
	ImGui::TableSetColumnIndex(1);
	ImGui::PushID(id);
	const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;
	const std::string sectionLabel = EditorIcons::MakeIconLabel(EditorIcons::EditorIcon::FolderOpen, label);
	bOpen = ImGui::TreeNodeEx("##section", flags, "%s (%zu)", sectionLabel.c_str(), count);
	ImGui::PopID();
	ImGui::TableSetColumnIndex(2);
	const std::string typeLabel = EditorIcons::MakeIconLabel(EditorIcons::EditorIcon::Folder, "Folder");
	EditorWidgets::DrawMutedText(typeLabel.c_str(), 0.58f);
	if (bOpen)
	{
		ImGui::TreePop();
	}
}

void SceneOutlinerPanel::DrawSelectionCell(const char* label, const SceneObjectSelection& selection, bool bIsSelected, int flags) noexcept
{
	if (bIsSelected)
	{
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	}
	const ImVec2 selectionSize(ImGui::GetContentRegionAvail().x, 0.0f);
	if (ImGui::Selectable(label, false, flags, selectionSize))
	{
		m_scene.Select(selection);
	}
	if (bIsSelected)
	{
		ImGui::PopStyleColor(2);
	}
}

void SceneOutlinerPanel::DrawSelectionEntry(const char* label, const char* typeLabel, const SceneObjectSelection& selection) noexcept
{
	const bool bIsSelected = m_scene.GetSelection() == selection;
	const bool bIsVisible = IsEntryVisible(selection);
	ImGui::TableNextRow();
	if (bIsSelected)
	{
		const ImU32 selectionColor = ImGui::ColorConvertFloat4ToU32(SparkleUiPalette::SelectionOverlay());
		ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, selectionColor);
		ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, selectionColor);
	}
	ImGui::PushID(label);
	ImGui::TableSetColumnIndex(0);
	if (EditorWidgets::DrawCenteredVisibilityIconButton("visibility", bIsVisible))
	{
		m_scene.ToggleVisibility(selection);
	}

	ImGui::TableSetColumnIndex(1);
	ImGui::Indent(16.0f);
	const EditorSceneEntry* entry = m_scene.GetModel()->FindEntry(selection);
	const SceneLightKind lightKind = entry ? entry->LightKind : SceneLightKind::Unknown;
	const EditorIcons::EditorIcon icon = SceneObjectPresentation::BuildSelectionIcon(selection, lightKind);
	EditorWidgets::DrawEditorIcon(icon, typeLabel, !bIsSelected);
	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
	{
		m_scene.Select(selection);
	}
	ImGui::SameLine(0.0f, 6.0f);
	if (!bIsVisible)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, SparkleUiPalette::TextMuted());
	}
	else if (bIsSelected)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, SparkleUiPalette::TextPrimary());
	}
	DrawSelectionCell(label, selection, bIsSelected, ImGuiSelectableFlags_SpanAllColumns);
	if (!bIsVisible || bIsSelected)
	{
		ImGui::PopStyleColor();
	}
	ImGui::Unindent(16.0f);
	ImGui::TableSetColumnIndex(2);
	ImVec4 mutedTypeColor = SparkleUiPalette::TextMuted();
	mutedTypeColor.w *= 0.62f;
	ImGui::PushStyleColor(ImGuiCol_Text, mutedTypeColor);
	DrawSelectionCell(typeLabel, selection, bIsSelected, 0);
	ImGui::PopStyleColor();
	ImGui::PopID();
}

void SceneOutlinerPanel::BeginWindow() noexcept
{
	ImGuiIO& io = ImGui::GetIO();

	constexpr float kMinWidth = 220.0f;
	const float kMaxWidth = (std::max) (kMinWidth + 1.0f, io.DisplaySize.x * 0.5f);
	m_widthPixels = std::clamp(m_widthPixels, kMinWidth, kMaxWidth);
	const float panelHeight = (std::max) (1.0f, io.DisplaySize.y - m_topInsetPixels);

	ImGui::SetNextWindowPos(ImVec2(0.0f, m_topInsetPixels), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(m_widthPixels, panelHeight), ImGuiCond_Once);
	ImGui::SetNextWindowSizeConstraints(ImVec2(kMinWidth, panelHeight), ImVec2(kMaxWidth, panelHeight));

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Scene Outliner", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
	ImGui::PopStyleVar();

	m_widthPixels = ImGui::GetWindowWidth();
}

void SceneOutlinerPanel::BuildSceneTable() noexcept
{
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4.0f, 2.0f));
	constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY | ImGuiTableFlags_NoPadOuterX;
	const bool bTableOpen = ImGui::BeginTable("##OutlinerTable", 3, tableFlags, ImVec2(0.0f, -28.0f));

	if (bTableOpen)
	{
		const EditorSceneModel& model = *m_scene.GetModel();
		ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 20.0f);
		ImGui::TableSetupColumn("Item Label", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 88.0f);
		ImGui::TableHeadersRow();

		DrawEntrySection(SceneOutlinerFilter::Cameras, "Camera", "Camera", "No cameras in scene", model.GetCameraEntries());
		DrawEntrySection(SceneOutlinerFilter::All, "Sky", "Sky", "", std::span(&model.GetSkyEntry(), 1));
		DrawEntrySection(SceneOutlinerFilter::Lights, "Lights", "Lights", "No lights in scene", model.GetLightEntries());
		DrawEntrySection(SceneOutlinerFilter::Meshes, "Meshes", "Meshes", "No meshes in scene", model.GetMeshEntries());
		ImGui::EndTable();
	}
	ImGui::PopStyleVar();
}

void SceneOutlinerPanel::BuildUI(bool bDisableInteraction)
{
	BeginWindow();
	EditorWidgets::DrawPanelHeader("Scene", "Outliner");

	if (m_scene.GetModel() == nullptr)
	{
		ImGui::TextDisabled("Scene outliner unavailable");
		ImGui::End();
		return;
	}

	ImGui::BeginDisabled(bDisableInteraction);
	BuildToolbar();
	BuildSceneTable();
	BuildFooter();
	ImGui::EndDisabled();

	ImGui::End();
}
