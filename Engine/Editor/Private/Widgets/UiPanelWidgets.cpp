#include "PCH.h"
#include "Widgets/UiPanelWidgets.h"

#include "Style/SparkleUiPalette.h"
#include "Style/SparkleUiTheme.h"

#include <imgui.h>

namespace EditorWidgets
{
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
		const ImVec2 borderStart(start.x, end.y - 1.0f);
		const ImVec2 borderEnd(end.x, end.y - 1.0f);
		drawList->AddLine(borderStart, borderEnd, borderColor, 1.0f);

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

	void DrawPanelHeader(const char* title, const char* subtitle)
	{
		ImFont* headingFont = SparkleUiTheme::GetHeadingFont();
		ImFont* monoFont = SparkleUiTheme::GetMonoFont();
		DrawHeaderBar(title, subtitle, 24.0f, SparkleUiPalette::PanelHeaderBackground(), SparkleUiPalette::PanelHeaderBorder(), headingFont, monoFont, ImVec2(8.0f, 3.0f));
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
