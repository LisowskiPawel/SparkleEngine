#include "PCH.h"
#include "Icons/EditorIcon.h"
#include "Widgets/UiIconWidgets.h"

#include "Style/SparkleUiPalette.h"

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

		const ImVec2 textSize = ImGui::CalcTextSize(text);
		const ImVec2 textPos(start.x + ((size.x - textSize.x) * 0.5f), start.y + ((size.y - textSize.y) * 0.5f));
		drawList->AddText(textPos, SparkleUiPalette::SceneOutlinerBadgeText(), text);

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
			const ImU32 backgroundColor = ImGui::ColorConvertFloat4ToU32(SparkleUiPalette::ButtonBackgroundHovered());
			drawList->AddRectFilled(start, end, backgroundColor, 3.0f);
		}

		const ImVec4 iconColor = bVisible ? SparkleUiPalette::WithAlpha(SparkleUiPalette::TextMuted(), 0.58f) : SparkleUiPalette::AccentStrong();
		const ImU32 iconColorU32 = ImGui::ColorConvertFloat4ToU32(iconColor);
		const EditorIcons::EditorIcon icon = bVisible ? EditorIcons::EditorIcon::EyeVisible : EditorIcons::EditorIcon::EyeHidden;
		const char* glyph = EditorIcons::GetEditorIconGlyph(icon);
		DrawCenteredGlyph(drawList, start, size, glyph, iconColorU32);

		if (bHovered)
		{
			ImGui::SetTooltip("%s", bVisible ? "Visible" : "Hidden");
		}
		ImGui::PopID();
		return bPressed;
	}

}
