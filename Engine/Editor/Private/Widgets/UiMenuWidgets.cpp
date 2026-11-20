#include "PCH.h"
#include "Icons/EditorIcon.h"
#include "Widgets/UiMenuWidgets.h"

#include "Style/SparkleUiPalette.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>

namespace EditorWidgets
{
	constexpr float menuHorizontalPadding = 0.6f;
	constexpr float menuVerticalPadding = 0.4f;
	constexpr float menuHorizontalSpacing = 0.5f;
	constexpr float menuVerticalSpacing = 0.4f;
	constexpr float menuIconOffset = 0.2f;
	constexpr float menuLabelOffset = 1.9f;
	constexpr float menuCheckColumnWidth = 1.5f;
	constexpr float menuArrowColumnWidth = 1.2f;
	constexpr float menuHeadingScale = 0.8f;
	constexpr float menuHeadingTracking = 0.075f;
	constexpr float menuHeadingLineGap = 0.7f;
	constexpr float menuHeadingLineWidth = 1.5f;

	static const char* NextMenuHeadingGlyph(const char* character)
	{
		const char* next = character + 1;
		while ((static_cast<unsigned char>(*next) & 0xc0u) == 0x80u)
		{
			++next;
		}
		return next;
	}

	static void DrawMenuRowContent(ImDrawList& drawList, ImVec2 position, const char* label, EditorIcons::EditorIcon icon, bool bHasCheckColumn)
	{
		const float fontSize = ImGui::GetFontSize();
		const float checkOffset = bHasCheckColumn ? menuCheckColumnWidth : 0.0f;
		if (icon != EditorIcons::EditorIcon::None)
		{
			const ImVec2 iconPosition(position.x + (menuIconOffset + checkOffset) * fontSize, position.y);
			const ImU32 iconColor = ImGui::GetColorU32(SparkleUiPalette::Menu().Icon);
			const char* glyph = EditorIcons::GetEditorIconGlyph(icon);
			drawList.AddText(iconPosition, iconColor, glyph);
		}
		const char* labelEnd = std::strstr(label, "##");
		const ImVec2 labelPosition(position.x + (menuLabelOffset + checkOffset) * fontSize, position.y);
		const ImU32 labelColor = ImGui::GetColorU32(ImGuiCol_Text);
		drawList.AddText(labelPosition, labelColor, label, labelEnd);
	}

	MenuStyleScope::MenuStyleScope()
	{
		const float fontSize = ImGui::GetFontSize();
		const float horizontalPadding = std::round(menuHorizontalPadding * fontSize);
		const float verticalPadding = std::round(menuVerticalPadding * fontSize);
		const float horizontalSpacing = std::round(menuHorizontalSpacing * fontSize);
		const float verticalSpacing = std::round(menuVerticalSpacing * fontSize);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(horizontalPadding, verticalPadding));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(horizontalSpacing, verticalSpacing));
		ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 0.12f * fontSize);
		const SparkleUiPalette::MenuColors& colors = SparkleUiPalette::Menu();
		ImGui::PushStyleColor(ImGuiCol_PopupBg, colors.Background);
		ImGui::PushStyleColor(ImGuiCol_Text, colors.Text);
		ImGui::PushStyleColor(ImGuiCol_TextDisabled, colors.MutedText);
		ImGui::PushStyleColor(ImGuiCol_Header, colors.Hovered);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, colors.Hovered);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, colors.Active);
		ImGui::PushStyleColor(ImGuiCol_Separator, colors.Divider);
	}

	MenuStyleScope::~MenuStyleScope()
	{
		ImGui::PopStyleColor(7);
		ImGui::PopStyleVar(4);
	}

	float MeasureMenuRow(const char* label, MenuRowKind kind)
	{
		float columns = menuLabelOffset + 2.0f * menuHorizontalPadding;
		if (kind == MenuRowKind::Toggle)
		{
			columns += menuCheckColumnWidth;
		}
		else if (kind == MenuRowKind::Submenu)
		{
			columns += menuArrowColumnWidth;
		}
		const float labelWidth = ImGui::CalcTextSize(label, nullptr, true).x;
		const float columnWidth = columns * ImGui::GetFontSize();
		return std::ceil(labelWidth + columnWidth);
	}

	float MeasureMenuSection(const char* label)
	{
		const float fontSize = ImGui::GetFontSize();
		ImFont* font = ImGui::GetFont();
		const float headingSize = fontSize * menuHeadingScale;
		const float maximumTextWidth = (std::numeric_limits<float>::max)();
		float width = (menuHeadingLineGap + menuHeadingLineWidth + 2.0f * menuHorizontalPadding) * fontSize;
		for (const char* character = label; *character != '\0'; character = NextMenuHeadingGlyph(character))
		{
			const char* glyphEnd = NextMenuHeadingGlyph(character);
			const float glyphWidth = font->CalcTextSizeA(headingSize, maximumTextWidth, 0.0f, character, glyphEnd).x;
			width += glyphWidth + menuHeadingTracking * fontSize;
		}
		return std::ceil(width);
	}

	bool BeginMenuPopup(const char* id, ImVec2 anchor, float width)
	{
		const ImVec2 minimumSize(width, 0.0f);
		const ImVec2 maximumSize(width, ImGui::GetIO().DisplaySize.y);
		ImGui::SetNextWindowSizeConstraints(minimumSize, maximumSize);
		const float popupOffset = 0.15f * ImGui::GetFontSize();
		const ImVec2 popupPosition(anchor.x, anchor.y + popupOffset);
		ImGui::SetNextWindowPos(popupPosition, ImGuiCond_Appearing);
		return ImGui::BeginPopup(id);
	}

	bool BeginMenu(const char* label, EditorIcons::EditorIcon icon, float childWidth, bool bEnabled)
	{
		const ImVec2 position = ImGui::GetCursorScreenPos();
		const float width = ImGui::GetContentRegionAvail().x;
		const float fontSize = ImGui::GetFontSize();
		ImDrawList& drawList = *ImGui::GetWindowDrawList();
		const ImVec2 minimumSize(childWidth, 0.0f);
		const ImVec2 maximumSize(childWidth, ImGui::GetIO().DisplaySize.y);
		ImGui::SetNextWindowSizeConstraints(minimumSize, maximumSize);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		const bool bOpen = ImGui::BeginMenu(label, bEnabled);
		ImGui::PopStyleColor();
		ImGui::BeginDisabled(!bEnabled);
		DrawMenuRowContent(drawList, position, label, icon, false);
		const float arrowX = position.x + width - menuHorizontalPadding * fontSize;
		const float centerY = position.y + 0.5f * fontSize;
		const ImU32 arrowColor = ImGui::GetColorU32(SparkleUiPalette::Menu().Icon);
		const ImVec2 arrowTop(arrowX, centerY - 0.25f * fontSize);
		const ImVec2 arrowTip(arrowX + 0.25f * fontSize, centerY);
		const ImVec2 arrowBottom(arrowX, centerY + 0.25f * fontSize);
		drawList.AddLine(arrowTop, arrowTip, arrowColor, 0.09f * fontSize);
		drawList.AddLine(arrowTip, arrowBottom, arrowColor, 0.09f * fontSize);
		ImGui::EndDisabled();
		return bOpen;
	}

	bool DrawMenuItem(const char* label, EditorIcons::EditorIcon icon, MenuCheckState checkState, ImGuiSelectableFlags flags)
	{
		const ImVec2 position = ImGui::GetCursorScreenPos();
		const bool bHasCheckColumn = checkState != MenuCheckState::Hidden;
		const MenuRowKind kind = bHasCheckColumn ? MenuRowKind::Toggle : MenuRowKind::Action;
		const float rowWidth = MeasureMenuRow(label, kind);
		const float minimumWidth = rowWidth - 2.0f * ImGui::GetStyle().WindowPadding.x;
		const float width = (std::max) (minimumWidth, ImGui::GetContentRegionAvail().x);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		const bool bPressed = ImGui::Selectable(label, false, flags, ImVec2(width, ImGui::GetFontSize()));
		ImGui::PopStyleColor();
		ImDrawList& drawList = *ImGui::GetWindowDrawList();
		DrawMenuRowContent(drawList, position, label, icon, bHasCheckColumn);
		const float fontSize = ImGui::GetFontSize();
		const float centerY = position.y + 0.5f * fontSize;
		const ImU32 color = ImGui::GetColorU32(ImGuiCol_Text);
		if (checkState == MenuCheckState::Checked)
		{
			const ImVec2 checkStart(position.x + 0.1f * fontSize, centerY);
			const ImVec2 checkCorner(position.x + 0.35f * fontSize, centerY + 0.25f * fontSize);
			const ImVec2 checkEnd(position.x + 0.85f * fontSize, centerY - 0.3f * fontSize);
			drawList.AddLine(checkStart, checkCorner, color, 0.11f * fontSize);
			drawList.AddLine(checkCorner, checkEnd, color, 0.11f * fontSize);
		}
		else if (checkState == MenuCheckState::Mixed)
		{
			const ImVec2 dashStart(position.x + 0.1f * fontSize, centerY);
			const ImVec2 dashEnd(position.x + 0.85f * fontSize, centerY);
			drawList.AddLine(dashStart, dashEnd, color, 0.11f * fontSize);
		}
		return bPressed;
	}

	void DrawMenuSection(const char* label)
	{
		const float fontSize = ImGui::GetFontSize();
		const ImVec2 position = ImGui::GetCursorScreenPos();
		const float width = ImGui::GetContentRegionAvail().x;
		ImDrawList& drawList = *ImGui::GetWindowDrawList();
		const float headingSize = fontSize * menuHeadingScale;
		ImFont* font = ImGui::GetFont();
		const ImU32 headingColor = ImGui::GetColorU32(SparkleUiPalette::Menu().Heading);
		const float maximumTextWidth = (std::numeric_limits<float>::max)();
		float textX = position.x;
		for (const char* character = label; *character != '\0'; character = NextMenuHeadingGlyph(character))
		{
			const char* glyphEnd = NextMenuHeadingGlyph(character);
			const ImVec2 glyphPosition(textX, position.y + 0.1f * fontSize);
			drawList.AddText(font, headingSize, glyphPosition, headingColor, character, glyphEnd);
			const float glyphWidth = font->CalcTextSizeA(headingSize, maximumTextWidth, 0.0f, character, glyphEnd).x;
			textX += glyphWidth + menuHeadingTracking * fontSize;
		}
		const float lineStart = textX + menuHeadingLineGap * fontSize;
		if (lineStart < position.x + width)
		{
			const ImVec2 dividerStart(lineStart, position.y + 0.5f * fontSize);
			const ImVec2 dividerEnd(position.x + width, position.y + 0.5f * fontSize);
			const ImU32 dividerColor = ImGui::GetColorU32(SparkleUiPalette::Menu().Divider);
			drawList.AddLine(dividerStart, dividerEnd, dividerColor, fontSize / 14.0f);
		}
		const float sectionHeight = std::round(0.9f * fontSize);
		ImGui::Dummy(ImVec2(width, sectionHeight));
	}
}
