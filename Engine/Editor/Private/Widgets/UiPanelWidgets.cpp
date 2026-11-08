#include "PCH.h"
#include "Widgets/UiPanelWidgets.h"

#include "Style/SparkleUiPalette.h"
#include "Style/SparkleUiTheme.h"

#include <imgui.h>

#include <algorithm>

namespace EditorWidgets
{
	static constexpr float PropertyLabelWidth = 78.0f;

	static void PushFontIfAvailable(ImFont* font)
	{
		if (font != nullptr)
		{
			ImGui::PushFont(font);
		}
	}

	static void PopFontIfAvailable(ImFont* font)
	{
		if (font != nullptr)
		{
			ImGui::PopFont();
		}
	}

	static void DrawHeaderBar(const char* title, const char* trailingText, float height, ImU32 backgroundColor, ImU32 borderColor, ImFont* titleFont, ImFont* trailingFont, const ImVec2& padding)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		const ImVec2 start = ImGui::GetCursorScreenPos();
		const float width = ImGui::GetContentRegionAvail().x;
		const ImVec2 end = ImVec2(start.x + width, start.y + height);

		drawList->AddRectFilled(start, end, backgroundColor);
		drawList->AddLine(ImVec2(start.x, end.y - 1.0f), ImVec2(end.x, end.y - 1.0f), borderColor, 1.0f);

		ImGui::InvisibleButton("##header_bar", ImVec2(width, height));
		ImGui::SetCursorScreenPos(ImVec2(start.x + padding.x, start.y + padding.y));
		PushFontIfAvailable(titleFont);
		ImGui::TextUnformatted(title);
		PopFontIfAvailable(titleFont);

		if (trailingText != nullptr && trailingText[0] != '\0')
		{
			const float trailingWidth = ImGui::CalcTextSize(trailingText).x;
			const float trailingRightPadding = padding.x + ImGui::GetStyle().ScrollbarSize;
			ImGui::SetCursorScreenPos(ImVec2(end.x - trailingRightPadding - trailingWidth, start.y + padding.y));
			PushFontIfAvailable(trailingFont);
			ImGui::TextDisabled("%s", trailingText);
			PopFontIfAvailable(trailingFont);
		}

		ImGui::SetCursorScreenPos(ImVec2(start.x, end.y + 4.0f));
	}

	static void DrawRightAlignedText(const char* value)
	{
		const float valueWidth = ImGui::CalcTextSize(value).x;
		const float currentX = ImGui::GetCursorPosX();
		const float offset = (std::max) (0.0f, ImGui::GetContentRegionAvail().x - valueWidth);
		ImGui::SetCursorPosX(currentX + offset);
		ImGui::TextUnformatted(value);
	}

	void DrawPanelHeader(const char* title, const char* subtitle)
	{
		ImFont* headingFont = SparkleUiTheme::GetHeadingFont();
		ImFont* monoFont = SparkleUiTheme::GetMonoFont();
		DrawHeaderBar(title, subtitle, 24.0f, SparkleUiPalette::PanelHeaderBackground(), SparkleUiPalette::PanelHeaderBorder(), headingFont, monoFont, ImVec2(8.0f, 3.0f));
	}

	void DrawKeyValueRow(const char* label, const char* value)
	{
		ImFont* monoFont = SparkleUiTheme::GetMonoFont();
		ImGui::PushID(label);
		const bool bPropertyTableOpen = ImGui::BeginTable("##kv_row", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_NoPadInnerX);

		if (bPropertyTableOpen)
		{
			ImGui::TableSetupColumn("label", ImGuiTableColumnFlags_WidthFixed, PropertyLabelWidth);
			ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			PushFontIfAvailable(monoFont);
			ImGui::TextDisabled("%s", label);
			PopFontIfAvailable(monoFont);

			ImGui::TableSetColumnIndex(1);
			PushFontIfAvailable(monoFont);
			DrawRightAlignedText(value);
			PopFontIfAvailable(monoFont);

			ImGui::EndTable();
		}
		ImGui::PopID();
	}

	void DrawSectionHeader(const char* title)
	{
		ImFont* headingFont = SparkleUiTheme::GetHeadingFont();
		ImGui::PushID(title);
		DrawHeaderBar(title, nullptr, 22.0f, SparkleUiPalette::SectionHeaderBackground(), SparkleUiPalette::SectionHeaderBorder(), headingFont, nullptr, ImVec2(6.0f, 3.0f));
		ImGui::PopID();
	}

	bool DrawFilterChip(const char* label, bool bActive) noexcept
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 2.0f));
		if (bActive)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, SparkleUiPalette::ButtonBackgroundActive());
			ImGui::PushStyleColor(ImGuiCol_Text, SparkleUiPalette::TextPrimary());
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, SparkleUiPalette::ButtonBackground());
			ImGui::PushStyleColor(ImGuiCol_Text, SparkleUiPalette::TextMuted());
		}

		ImGui::PushID("FilterChip");
		const bool bPressed = ImGui::SmallButton(label);
		ImGui::PopID();
		ImGui::PopStyleColor(2);
		ImGui::PopStyleVar();
		return bPressed;
	}

	void DrawMutedText(const char* text, float alpha) noexcept
	{
		ImVec4 color = SparkleUiPalette::TextMuted();
		color.w *= alpha;
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::TextUnformatted(text);
		ImGui::PopStyleColor();
	}
}
