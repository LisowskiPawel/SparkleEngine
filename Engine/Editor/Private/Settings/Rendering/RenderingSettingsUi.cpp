#include "PCH.h"

#include "Settings/Rendering/RenderingSettingsUi.h"

#include "Style/SparkleUiPalette.h"
#include "Widgets/UiDetailsWidgets.h"

#include <string>

namespace RenderingSettingsUi
{
	bool MatchesFilter(const char* filterText, const char* title, const char* keywords)
	{
		if (filterText == nullptr || filterText[0] == '\0')
		{
			return true;
		}

		return EditorWidgets::MatchesDetailsFilter(std::string(filterText), title, keywords);
	}

	bool BeginSettingsCategory(const char* label)
	{
		ImGui::PushStyleColor(ImGuiCol_Header, SparkleUiPalette::HeaderBackground());
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, SparkleUiPalette::HeaderBackgroundHovered());
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, SparkleUiPalette::HeaderBackgroundActive());
		ImGui::PushStyleColor(ImGuiCol_Text, SparkleUiPalette::TextPrimary());
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 5.0f));

		const bool bOpen = ImGui::CollapsingHeader(label, ImGuiTreeNodeFlags_DefaultOpen);

		ImGui::PopStyleVar();
		ImGui::PopStyleColor(4);
		return bOpen;
	}

	bool BeginSettingsTable(const char* id)
	{
		const ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersInnerH;
		if (!ImGui::BeginTable(id, 2, tableFlags))
		{
			return false;
		}

		ImGui::TableSetupColumn("Setting", ImGuiTableColumnFlags_WidthFixed, kLabelColumnWidth);
		ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
		return true;
	}

	void BeginSettingsRow(const char* label)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label);
	}

	bool DrawBooleanRow(const char* id, const char* label, bool& bValue)
	{
		BeginSettingsRow(label);
		ImGui::TableSetColumnIndex(1);
		return ImGui::Checkbox(id, &bValue);
	}

	bool DrawUnsignedIntSliderRow(const char* id, const char* label, std::uint32_t& value, std::uint32_t minValue, std::uint32_t maxValue)
	{
		BeginSettingsRow(label);
		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);

		int updatedValue = static_cast<int>(value);
		const int minInt = static_cast<int>(minValue);
		const int maxInt = static_cast<int>(maxValue);
		const bool bEdited = ImGui::SliderInt(id, &updatedValue, minInt, maxInt);
		if (!bEdited)
		{
			return false;
		}

		value = static_cast<std::uint32_t>(updatedValue);
		return true;
	}

	bool DrawFloatInputRow(const char* id, const char* label, float& value, float step, float stepFast, const char* format)
	{
		BeginSettingsRow(label);
		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);
		return ImGui::InputFloat(id, &value, step, stepFast, format);
	}

}
