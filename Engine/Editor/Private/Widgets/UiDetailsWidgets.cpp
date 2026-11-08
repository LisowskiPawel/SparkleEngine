#include "PCH.h"
#include "Core/Public/Strings/StringUtils.h"
#include "Widgets/UiDetailsWidgets.h"
#include "Widgets/UiIconWidgets.h"

#include "Style/SparkleUiPalette.h"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <vector>

#include <imgui.h>

namespace EditorWidgets
{
	static constexpr float DetailsLabelWidth = 132.0f;
	static constexpr float DetailsAxisLabelWidth = 12.0f;
	static constexpr float DetailsRowVerticalPadding = 1.0f;
	static constexpr float DetailsUtilityColumnWidth = 24.0f;
	static constexpr float DetailsResetButtonSize = 14.0f;
	static constexpr float DetailsDirtyEpsilon = 0.0001f;

	static ImVec4 DetailsGridLineColor() noexcept
	{
		ImVec4 color = ImGui::ColorConvertU32ToFloat4(SparkleUiPalette::PanelHeaderBorder());
		color.w = 0.18f;
		return color;
	}

	static ImU32 DetailsRowBackgroundColor() noexcept
	{
		return IM_COL32(24, 25, 28, 150);
	}

	static ImU32 AxisColor(int axisIndex) noexcept
	{
		switch (axisIndex)
		{
			case 0:
				return IM_COL32(205, 82, 82, 210);
			case 1:
				return IM_COL32(82, 178, 104, 210);
			case 2:
				return IM_COL32(82, 122, 208, 210);
			default:
				return ImGui::ColorConvertFloat4ToU32(SparkleUiPalette::TextMuted());
		}
	}

	static bool IsDifferentFromDefault(float value, float defaultValue) noexcept
	{
		return std::fabs(value - defaultValue) > DetailsDirtyEpsilon;
	}

	static bool IsDifferentFromDefault(const float values[3], const float defaultValues[3]) noexcept
	{
		for (int axis = 0; axis < 3; ++axis)
		{
			if (IsDifferentFromDefault(values[axis], defaultValues[axis]))
			{
				return true;
			}
		}
		return false;
	}

	static bool BeginDetailsRow(const char* label, int valueColumnCount)
	{
		ImGui::PushID(label);
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, DetailsRowVerticalPadding));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 2.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
		const ImVec4 gridLineColor = DetailsGridLineColor();
		ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, gridLineColor);
		ImGui::PushStyleColor(ImGuiCol_TableBorderLight, gridLineColor);
		ImGui::PushStyleColor(ImGuiCol_Separator, gridLineColor);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.18f, 0.19f, 0.21f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.22f, 0.23f, 0.25f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.18f, 0.29f, 0.45f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_CheckMark, SparkleUiPalette::AccentStrong());

		ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingStretchSame;
		tableFlags |= ImGuiTableFlags_NoPadOuterX;
		tableFlags |= ImGuiTableFlags_BordersInnerV;

		if (!ImGui::BeginTable("##details_row", valueColumnCount + 2, tableFlags))
		{
			ImGui::PopStyleColor(7);
			ImGui::PopStyleVar(5);
			ImGui::PopID();
			return false;
		}

		ImGui::TableSetupColumn("label", ImGuiTableColumnFlags_WidthFixed, DetailsLabelWidth);
		for (int valueColumnIndex = 0; valueColumnIndex < valueColumnCount; ++valueColumnIndex)
		{
			ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthStretch);
		}
		ImGui::TableSetupColumn("utility", ImGuiTableColumnFlags_WidthFixed, DetailsUtilityColumnWidth);

		ImGui::TableNextRow();
		ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, DetailsRowBackgroundColor());
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::TextDisabled("%s", label);
		return true;
	}

	static bool DrawDetailsResetButton(int utilityColumnIndex, bool bDirty, const char* tooltip = "Reset to default")
	{
		ImGui::TableSetColumnIndex(utilityColumnIndex);
		if (!bDirty)
		{
			ImGui::Dummy(ImVec2(DetailsUtilityColumnWidth, DetailsResetButtonSize));
			return false;
		}

		ImGui::PushID("reset");
		const ImVec2 start = ImGui::GetCursorScreenPos();
		const ImVec2 size(DetailsResetButtonSize, DetailsResetButtonSize);
		const bool bPressed = ImGui::InvisibleButton("##reset", size);
		const bool bHovered = ImGui::IsItemHovered();
		const bool bActive = ImGui::IsItemActive();

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		const ImVec2 end(start.x + size.x, start.y + size.y);
		if (bHovered || bActive)
		{
			drawList->AddRectFilled(start, end, ImGui::ColorConvertFloat4ToU32(SparkleUiPalette::ButtonBackgroundHovered()), 3.0f);
		}

		const ImVec4 iconColor = bHovered ? SparkleUiPalette::TextPrimary() : SparkleUiPalette::WithAlpha(SparkleUiPalette::TextMuted(), 0.62f);
		const ImU32 iconColorU32 = ImGui::ColorConvertFloat4ToU32(iconColor);
		DrawCenteredGlyph(drawList, start, size, EditorIcons::GetEditorIconGlyph(EditorIcons::EditorIcon::Reset), iconColorU32);
		if (tooltip != nullptr && tooltip[0] != '\0' && bHovered)
		{
			ImGui::SetTooltip("%s", tooltip);
		}

		ImGui::PopID();
		return bPressed;
	}

	static void DrawDetailsEmptyUtility(int utilityColumnIndex)
	{
		ImGui::TableSetColumnIndex(utilityColumnIndex);
		ImGui::Dummy(ImVec2(DetailsUtilityColumnWidth, EditorIconSize));
	}

	static void EndDetailsRow()
	{
		ImGui::EndTable();
		ImGui::Separator();
		ImGui::PopStyleColor(7);
		ImGui::PopStyleVar(5);
		ImGui::PopID();
	}

	bool DrawDetailsCategory(const char* title, bool bDefaultOpen)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed;
		if (bDefaultOpen)
		{
			flags |= ImGuiTreeNodeFlags_DefaultOpen;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 4.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Header, ImGui::ColorConvertU32ToFloat4(SparkleUiPalette::SectionHeaderBackground()));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.21f, 0.22f, 0.24f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.19f, 0.26f, 0.36f, 1.0f));
		const bool bOpen = ImGui::CollapsingHeader(title, flags);
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(2);
		return bOpen;
	}

	void DrawDetailsEmptyState(const char* text)
	{
		ImGui::TextDisabled("%s", text);
	}

	void DrawDetailsValueRow(const char* label, const char* value)
	{
		if (!BeginDetailsRow(label, 1))
		{
			return;
		}

		ImGui::TableSetColumnIndex(1);
		ImGui::AlignTextToFramePadding();
		ImGui::TextWrapped("%s", value);
		DrawDetailsEmptyUtility(2);

		EndDetailsRow();
	}

	bool EditDetailsFloat(const char* label, float& value, float speed, float minValue, float maxValue, const char* format, const float* resetValue)
	{
		if (!BeginDetailsRow(label, 1))
		{
			return false;
		}

		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);
		bool bChanged = ImGui::DragFloat("##value", &value, speed, minValue, maxValue, format);
		if (resetValue != nullptr)
		{
			if (DrawDetailsResetButton(2, IsDifferentFromDefault(value, *resetValue)))
			{
				value = *resetValue;
				bChanged = true;
			}
		}
		else
		{
			DrawDetailsEmptyUtility(2);
		}

		EndDetailsRow();
		return bChanged;
	}

	bool EditDetailsFloat3(const char* label, float values[3], float speed, float minValue, float maxValue, const char* format, const float* resetValues)
	{
		if (!BeginDetailsRow(label, 3))
		{
			return false;
		}

		bool bChanged = false;
		static constexpr const char* axisLabels[] = {"X", "Y", "Z"};
		for (int axisIndex = 0; axisIndex < 3; ++axisIndex)
		{
			ImGui::PushID(axisIndex);
			ImGui::TableSetColumnIndex(axisIndex + 1);
			const bool bAxisTableOpen = ImGui::BeginTable("##axis_field", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_NoPadInnerX);

			if (bAxisTableOpen)
			{
				ImGui::TableSetupColumn("axis", ImGuiTableColumnFlags_WidthFixed, DetailsAxisLabelWidth);
				ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				ImGui::PushStyleColor(ImGuiCol_Text, AxisColor(axisIndex));
				ImGui::TextUnformatted(axisLabels[axisIndex]);
				ImGui::PopStyleColor();

				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(-FLT_MIN);
				bChanged = ImGui::DragFloat("##value", &values[axisIndex], speed, minValue, maxValue, format) || bChanged;

				ImGui::EndTable();
			}
			ImGui::PopID();
		}
		if (resetValues != nullptr)
		{
			if (DrawDetailsResetButton(4, IsDifferentFromDefault(values, resetValues)))
			{
				values[0] = resetValues[0];
				values[1] = resetValues[1];
				values[2] = resetValues[2];
				bChanged = true;
			}
		}
		else
		{
			DrawDetailsEmptyUtility(4);
		}

		EndDetailsRow();
		return bChanged;
	}

	bool EditDetailsColor3(const char* label, float values[3], const float* resetValues)
	{
		if (!BeginDetailsRow(label, 1))
		{
			return false;
		}

		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);
		bool bChanged = ImGui::ColorEdit3("##value", values, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar);
		if (resetValues != nullptr)
		{
			if (DrawDetailsResetButton(2, IsDifferentFromDefault(values, resetValues)))
			{
				values[0] = resetValues[0];
				values[1] = resetValues[1];
				values[2] = resetValues[2];
				bChanged = true;
			}
		}
		else
		{
			DrawDetailsEmptyUtility(2);
		}

		EndDetailsRow();
		return bChanged;
	}

	bool EditDetailsCheckbox(const char* label, bool& bValue, const bool* resetValue)
	{
		if (!BeginDetailsRow(label, 1))
		{
			return false;
		}

		ImGui::TableSetColumnIndex(1);
		bool bChanged = ImGui::Checkbox("##value", &bValue);
		if (resetValue != nullptr)
		{
			if (DrawDetailsResetButton(2, bValue != *resetValue))
			{
				bValue = *resetValue;
				bChanged = true;
			}
		}
		else
		{
			DrawDetailsEmptyUtility(2);
		}

		EndDetailsRow();
		return bChanged;
	}

	bool EditDetailsText(const char* label, std::string& value, const std::string* resetValue)
	{
		if (!BeginDetailsRow(label, 1))
		{
			return false;
		}

		std::vector<char> buffer((std::max) (std::size_t{1024}, value.size() + 1u), '\0');
		std::copy(value.begin(), value.end(), buffer.begin());
		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);
		bool bChanged = ImGui::InputText("##value", buffer.data(), buffer.size());
		if (bChanged)
		{
			value = buffer.data();
		}

		if (resetValue != nullptr)
		{
			if (DrawDetailsResetButton(2, value != *resetValue))
			{
				value = *resetValue;
				bChanged = true;
			}
		}
		else
		{
			DrawDetailsEmptyUtility(2);
		}

		EndDetailsRow();
		return bChanged;
	}

	bool MatchesDetailsFilter(const std::string& filterText, const char* title, const char* keywords) noexcept
	{
		return filterText.empty() || Strings::ContainsIgnoreCase(title, filterText) || Strings::ContainsIgnoreCase(keywords, filterText);
	}
}
