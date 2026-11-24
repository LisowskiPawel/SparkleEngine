#include "PCH.h"

#include "Settings/Rendering/RenderingSettingsUi.h"

#include "Style/SparkleUiPalette.h"
#include "Widgets/UiDetailsWidgets.h"

namespace RenderingSettingsUi
{
	bool BeginSettingsCategory(const char* filterText, const char* label, const char* keywords)
	{
		if (filterText != nullptr && !EditorWidgets::MatchesDetailsFilter(filterText, label, keywords))
		{
			return false;
		}

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

	void DrawBooleanRow(const char* id, const char* label, EngineRenderingSettingsController& controller, bool EngineRenderingSettingsState::* field)
	{
		BeginSettingsRow(label);
		ImGui::TableSetColumnIndex(1);
		bool bValue = controller.GetState().*field;
		if (ImGui::Checkbox(id, &bValue))
		{
			controller.Set(field, bValue);
		}
	}

	void DrawUnsignedIntSliderRow(
	    const char* id,
	    const char* label,
	    EngineRenderingSettingsController& controller,
	    std::uint32_t EngineRenderingSettingsState::* field,
	    std::uint32_t minValue,
	    std::uint32_t maxValue)
	{
		BeginSettingsRow(label);
		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);

		int updatedValue = static_cast<int>(controller.GetState().*field);
		const int minInt = static_cast<int>(minValue);
		const int maxInt = static_cast<int>(maxValue);
		if (ImGui::SliderInt(id, &updatedValue, minInt, maxInt))
		{
			controller.Set(field, static_cast<std::uint32_t>(updatedValue));
		}
	}

	void DrawFloatInputRow(const char* id, const char* label, EngineRenderingSettingsController& controller, float EngineRenderingSettingsState::* field, float step, float stepFast, const char* format)
	{
		BeginSettingsRow(label);
		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);
		float value = controller.GetState().*field;
		if (ImGui::InputFloat(id, &value, step, stepFast, format))
		{
			controller.Set(field, value);
		}
	}

}
