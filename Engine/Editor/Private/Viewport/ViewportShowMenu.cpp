#include "PCH.h"
#include "Viewport/ViewportShowMenu.h"
#include "Icons/EditorIcon.h"
#include "Widgets/UiMenuWidgets.h"

#include <imgui.h>

#include <array>
#include <algorithm>
#include <cstddef>
#include <span>
#include <utility>

struct ShowControl final
{
	const char* Label;
	const char* CVarName;
	EditorIcons::EditorIcon Icon;
};

static constexpr std::array<ShowControl, 15> showControls = {
    {{"Sky", "r.Sky.Enabled", EditorIcons::EditorIcon::Sky},
        {"Static Meshes", "r.Meshes.Static", EditorIcons::EditorIcon::StaticMesh},
        {"Skinned Meshes", "r.Meshes.Skinned", EditorIcons::EditorIcon::SkinnedMesh},
        {"Emissive", "r.Lighting.Emissive", EditorIcons::EditorIcon::ViewEmissive},
        {"Directional Lights", "r.Lighting.Lights.Directional", EditorIcons::EditorIcon::DirectionalLight},
        {"Point Lights", "r.Lighting.Lights.Point", EditorIcons::EditorIcon::PointLight},
        {"Spot Lights", "r.Lighting.Lights.Spot", EditorIcons::EditorIcon::SpotLight},
        {"Rect Lights", "r.Lighting.Lights.Rect", EditorIcons::EditorIcon::RectLight},
        {"Diffuse", "r.Lighting.Direct.Diffuse", EditorIcons::EditorIcon::ViewDirectDiffuse},
        {"Specular", "r.Lighting.Direct.Specular", EditorIcons::EditorIcon::ViewDirectSpecular},
        {"Subsurface", "r.Lighting.Direct.Subsurface", EditorIcons::EditorIcon::ViewDirectSubsurface},
        {"Diffuse", "r.Lighting.Indirect.Diffuse", EditorIcons::EditorIcon::ViewDirectDiffuse},
        {"Specular", "r.Lighting.Indirect.Specular", EditorIcons::EditorIcon::ViewDirectSpecular},
        {"Direct Shadows", "r.Lighting.Shadows.Direct", EditorIcons::EditorIcon::ViewAmbientOcclusion},
        {"Indirect Shadows", "r.Lighting.Shadows.Indirect", EditorIcons::EditorIcon::ViewAmbientOcclusion}}};

struct ShowMenuEntry final
{
	const char* Label;
	EditorIcons::EditorIcon Icon;
	std::size_t FirstLeaf;
	std::size_t LeafCount;
	const char* Section;
};

static constexpr auto showMenuEntries = std::to_array<ShowMenuEntry>(
    {{nullptr, EditorIcons::EditorIcon::None, 0, 1, "COMMON SHOW FLAGS"},
        {nullptr, EditorIcons::EditorIcon::None, 1, 1, nullptr},
        {nullptr, EditorIcons::EditorIcon::None, 2, 1, nullptr},
        {nullptr, EditorIcons::EditorIcon::None, 3, 1, nullptr},
        {nullptr, EditorIcons::EditorIcon::None, 4, 1, "LIGHT TYPES"},
        {"Local Lights", EditorIcons::EditorIcon::PointLight, 5, 3, nullptr},
        {"Direct Lighting", EditorIcons::EditorIcon::DirectionalLight, 8, 3, "LIGHTING COMPONENTS"},
        {"Indirect Lighting", EditorIcons::EditorIcon::Light, 11, 2, nullptr},
        {"Shadows", EditorIcons::EditorIcon::ViewAmbientOcclusion, 13, 2, "LIGHTING FEATURES"}});

static bool QueryShowControlIntent(const CVarControlExecutor& executor, std::array<bool, showControls.size()>& intent, std::string& error)
{
	CVarControlRequest request;
	request.Entries.reserve(showControls.size());
	for (const ShowControl& leaf : showControls)
	{
		request.Entries.push_back({leaf.CVarName, {}});
	}
	const CVarControlResult result = executor(std::move(request));
	if (!result.Error.empty())
	{
		error = result.Error;
		return false;
	}
	if (result.Values.size() != showControls.size())
	{
		error = "Show control query returned an incomplete response.";
		return false;
	}
	for (std::size_t index = 0; index < showControls.size(); ++index)
	{
		const CVarControlValue& value = result.Values[index];
		if (value.Name != showControls[index].CVarName || value.Type != "bool" || (value.Value != "true" && value.Value != "false"))
		{
			error = "Invalid show control response: " + std::string(showControls[index].CVarName);
			return false;
		}
		intent[index] = value.Value == "true";
	}
	return true;
}

static void SetShowControlIntent(const CVarControlExecutor& executor, std::span<const ShowControl> leaves, bool bEnabled, std::string& error)
{
	CVarControlRequest request;
	request.Operation = CVarControlOperation::Set;
	request.Entries.reserve(leaves.size());
	for (const ShowControl& leaf : leaves)
	{
		request.Entries.push_back({leaf.CVarName, bEnabled ? "true" : "false"});
	}
	error = executor(std::move(request)).Error;
}

static void DrawShowControl(const ShowControl& control, bool bEnabled, const CVarControlExecutor& executor, std::string& error)
{
	const EditorWidgets::MenuCheckState state = bEnabled ? EditorWidgets::MenuCheckState::Checked : EditorWidgets::MenuCheckState::Unchecked;
	if (EditorWidgets::DrawMenuItem(control.Label, control.Icon, state, ImGuiSelectableFlags_NoAutoClosePopups))
	{
		SetShowControlIntent(executor, std::span(&control, 1), !bEnabled, error);
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
	{
		ImGui::SetTooltip("%s\nShared by all viewports.", control.CVarName);
	}
}

static void DrawShowControlGroup(
    const char* label,
    EditorIcons::EditorIcon icon,
    std::span<const ShowControl> leaves,
    std::span<const bool> intent,
    const CVarControlExecutor& executor,
    std::string& error)
{
	float menuWidth = EditorWidgets::MeasureMenuRow("All", EditorWidgets::MenuRowKind::Toggle);
	for (const ShowControl& leaf : leaves)
	{
		menuWidth = (std::max) (menuWidth, EditorWidgets::MeasureMenuRow(leaf.Label, EditorWidgets::MenuRowKind::Toggle));
	}
	if (!EditorWidgets::BeginMenu(label, icon, menuWidth))
	{
		return;
	}
	bool bAnyEnabled = false;
	bool bAllEnabled = true;
	for (bool bEnabled : intent)
	{
		bAnyEnabled |= bEnabled;
		bAllEnabled &= bEnabled;
	}

	const EditorWidgets::MenuCheckState groupState = bAllEnabled ? EditorWidgets::MenuCheckState::Checked
	    : bAnyEnabled                                            ? EditorWidgets::MenuCheckState::Mixed
	                                                             : EditorWidgets::MenuCheckState::Unchecked;

	if (EditorWidgets::DrawMenuItem("All", EditorIcons::EditorIcon::None, groupState, ImGuiSelectableFlags_NoAutoClosePopups))
	{
		SetShowControlIntent(executor, leaves, !bAnyEnabled, error);
	}
	if (bAnyEnabled && !bAllEnabled && ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Mixed selection. Toggle to disable all %s controls.", label);
	}
	ImGui::Separator();
	for (std::size_t index = 0; index < leaves.size(); ++index)
	{
		DrawShowControl(leaves[index], intent[index], executor, error);
	}
	ImGui::EndMenu();
}

static float MeasureShowMenuWidth()
{
	float menuWidth = EditorWidgets::MeasureMenuRow("Use Defaults", EditorWidgets::MenuRowKind::Action);
	for (const ShowMenuEntry& group : showMenuEntries)
	{
		const bool bIsSubmenu = group.LeafCount > 1;
		const char* label = bIsSubmenu ? group.Label : showControls[group.FirstLeaf].Label;
		const auto kind = bIsSubmenu ? EditorWidgets::MenuRowKind::Submenu : EditorWidgets::MenuRowKind::Toggle;
		menuWidth = (std::max) (menuWidth, EditorWidgets::MeasureMenuRow(label, kind));
		if (group.Section != nullptr)
		{
			menuWidth = (std::max) (menuWidth, EditorWidgets::MeasureMenuSection(group.Section));
		}
	}
	return menuWidth;
}

static void DrawShowMenuContents(const CVarControlExecutor* executor, bool bDisableInteraction, std::string& error)
{
	std::array<bool, showControls.size()> intent{};
	std::string queryError;
	const bool bAvailableExecutor = executor != nullptr && *executor;
	const bool bAvailable = bAvailableExecutor && (QueryShowControlIntent(*executor, intent, queryError));
	if (!bAvailable)
	{
		ImGui::TextWrapped("Show controls unavailable: %s", queryError.empty() ? "Host control executor is missing." : queryError.c_str());
	}
	else
	{
		ImGui::BeginDisabled(bDisableInteraction);
		const bool bUseDefaultsSelected = EditorWidgets::DrawMenuItem("Use Defaults", EditorIcons::EditorIcon::None, EditorWidgets::MenuCheckState::Hidden, ImGuiSelectableFlags_NoAutoClosePopups);

		if (bUseDefaultsSelected)
		{
			SetShowControlIntent(*executor, showControls, true, error);
		}
		for (const ShowMenuEntry& group : showMenuEntries)
		{
			if (group.Section != nullptr)
			{
				EditorWidgets::DrawMenuSection(group.Section);
			}
			if (group.LeafCount == 1)
			{
				DrawShowControl(showControls[group.FirstLeaf], intent[group.FirstLeaf], *executor, error);
				continue;
			}

			DrawShowControlGroup(
			    group.Label,
			    group.Icon,
			    std::span(showControls).subspan(group.FirstLeaf, group.LeafCount),
			    std::span(intent).subspan(group.FirstLeaf, group.LeafCount),
			    *executor,
			    error);
		}
		ImGui::EndDisabled();
	}
}

void DrawViewportShowMenu(const CVarControlExecutor* executor, bool bDisableInteraction, std::string& error)
{
	ImGui::BeginDisabled(bDisableInteraction);
	const std::string showLabel = EditorIcons::MakeIconLabel(EditorIcons::EditorIcon::EyeVisible, "Show");
	if (ImGui::Button(showLabel.c_str()))
	{
		ImGui::OpenPopup("ViewportShowMenu");
	}
	ImGui::EndDisabled();
	const ImVec2 popupPosition(ImGui::GetItemRectMin().x, ImGui::GetWindowPos().y + ImGui::GetWindowHeight());
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
	{
		ImGui::SetTooltip("Shared rendering controls for all viewports.");
	}
	if (!ImGui::IsPopupOpen("ViewportShowMenu"))
	{
		return;
	}
	const EditorWidgets::MenuStyleScope menuStyle;

	if (!EditorWidgets::BeginMenuPopup("ViewportShowMenu", popupPosition, MeasureShowMenuWidth()))
	{
		return;
	}
	DrawShowMenuContents(executor, bDisableInteraction, error);

	if (!error.empty())
	{
		ImGui::Separator();
		ImGui::TextWrapped("Show edit rejected: %s", error.c_str());
	}
	ImGui::EndPopup();
}
