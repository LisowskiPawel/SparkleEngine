#include "PCH.h"
#include "Widgets/UiPanelWidgets.h"

#include "Style/SparkleUiPalette.h"
#include "Style/SparkleUiTheme.h"

#include <imgui.h>

#include "Widgets/UiWidgetPrimitives.h"

namespace EditorWidgets
{
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
