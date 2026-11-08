#include "PCH.h"
#include "Icons/EditorIcon.h"
#include "Widgets/UiIconWidgets.h"

#include "Style/SparkleUiPalette.h"
#include "Style/SparkleUiTheme.h"

#include "Core/Public/Strings/StringUtils.h"

#include <algorithm>

#include <imgui.h>

namespace EditorWidgets
{
	void DrawCenteredGlyph(ImDrawList* drawList, const ImVec2& start, const ImVec2& size, const char* glyph, ImU32 color) noexcept
	{
		const ImVec2 textSize = ImGui::CalcTextSize(glyph);
		const ImVec2 textPosition(start.x + ((size.x - textSize.x) * 0.5f), start.y + ((size.y - textSize.y) * 0.5f));
		drawList->AddText(textPosition, color, glyph);
	}

	bool DrawEditorIconButton(EditorIcons::EditorIcon icon, const char* id, const char* tooltip)
	{
		ImGui::PushID(id);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, SparkleUiPalette::ButtonBackgroundHovered());
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, SparkleUiPalette::ButtonBackgroundActive());
		ImGui::PushStyleColor(ImGuiCol_Text, SparkleUiPalette::TextMuted());
		const bool bPressed = ImGui::Button(EditorIcons::GetEditorIconGlyph(icon), ImVec2(EditorIconSize, EditorIconSize));
		if (tooltip != nullptr && tooltip[0] != '\0' && ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("%s", tooltip);
		}
		ImGui::PopStyleColor(4);
		ImGui::PopStyleVar();
		ImGui::PopID();
		return bPressed;
	}

	bool DrawCenteredVisibilityIconButton(const char* id, bool bVisible) noexcept
	{
		const float availableWidth = ImGui::GetContentRegionAvail().x;
		const float horizontalOffset = (std::max) (0.0f, (availableWidth - EditorIconSize) * 0.5f);
		const float verticalOffset = (std::max) (0.0f, (ImGui::GetFrameHeight() - EditorIconSize) * 0.5f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + horizontalOffset);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + verticalOffset);
		return DrawVisibilityIconButton(id, bVisible);
	}

	void DrawEditorIcon(EditorIcons::EditorIcon icon, const char* tooltip, bool bDrawBadgeBackground)
	{
		const char* text = EditorIcons::GetEditorIconGlyph(icon);
		const ImVec2 size(EditorIconSize, EditorIconSize);
		const ImVec2 start = ImGui::GetCursorScreenPos();
		ImGui::InvisibleButton("##placeholder_type_icon", size);

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		const ImVec2 end(start.x + size.x, start.y + size.y);
		if (bDrawBadgeBackground)
		{
			drawList->AddRectFilled(start, end, SparkleUiPalette::SceneOutlinerBadgeBackground(), 3.0f);
			drawList->AddRect(start, end, SparkleUiPalette::PanelHeaderBorder(), 3.0f, 0, 1.0f);
		}

		if (text != nullptr && text[0] != '\0')
		{
			const ImVec2 textSize = ImGui::CalcTextSize(text);
			const ImVec2 textPos(start.x + ((size.x - textSize.x) * 0.5f), start.y + ((size.y - textSize.y) * 0.5f));
			drawList->AddText(textPos, SparkleUiPalette::SceneOutlinerBadgeText(), text);
		}

		if (tooltip != nullptr && tooltip[0] != '\0' && ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("%s", tooltip);
		}
	}

	bool DrawVisibilityIconButton(const char* id, bool bVisible)
	{
		ImGui::PushID(id);
		const ImVec2 start = ImGui::GetCursorScreenPos();
		const ImVec2 size(EditorIconSize, EditorIconSize);
		const bool bPressed = ImGui::InvisibleButton("##visibility", size);
		const bool bHovered = ImGui::IsItemHovered();
		const bool bActive = ImGui::IsItemActive();

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		const ImVec2 end(start.x + size.x, start.y + size.y);
		if (bHovered || bActive)
		{
			drawList->AddRectFilled(start, end, ImGui::ColorConvertFloat4ToU32(SparkleUiPalette::ButtonBackgroundHovered()), 3.0f);
		}

		const ImVec4 iconColor = bVisible ? SparkleUiPalette::WithAlpha(SparkleUiPalette::TextMuted(), 0.58f) : SparkleUiPalette::AccentStrong();
		const ImU32 iconColorU32 = ImGui::ColorConvertFloat4ToU32(iconColor);
		DrawCenteredGlyph(drawList, start, size, EditorIcons::GetEditorIconGlyph(bVisible ? EditorIcons::EditorIcon::EyeVisible : EditorIcons::EditorIcon::EyeHidden), iconColorU32);

		if (bHovered)
		{
			ImGui::SetTooltip("%s", bVisible ? "Visible" : "Hidden");
		}
		ImGui::PopID();
		return bPressed;
	}

}
