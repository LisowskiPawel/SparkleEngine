#include "PCH.h"
#include "Core/Public/Strings/StringUtils.h"
#include "Widgets/UiDetailsWidgets.h"

#include "Style/SparkleUiPalette.h"

#include <algorithm>
#include <cfloat>
#include <vector>

#include <imgui.h>

#include "Widgets/UiWidgetPrimitives.h"

namespace EditorWidgets
{
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
