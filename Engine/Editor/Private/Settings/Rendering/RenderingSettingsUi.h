#pragma once

#include "Settings/EngineRenderingSettingsController.h"

#include <imgui.h>

#include <cstddef>
#include <cfloat>
#include <cstdint>

namespace RenderingSettingsUi
{
	inline constexpr float kLabelColumnWidth = 340.0f;

	template <typename ValueType> struct ComboOption final
	{
		const char* Label = "";
		ValueType Value{};
	};

	bool BeginSettingsCategory(const char* filterText, const char* label, const char* keywords);
	bool BeginSettingsTable(const char* id);
	void BeginSettingsRow(const char* label);

	void DrawBooleanRow(const char* id, const char* label, EngineRenderingSettingsController& controller, bool EngineRenderingSettingsState::* field);
	void DrawUnsignedIntSliderRow(
	    const char* id,
	    const char* label,
	    EngineRenderingSettingsController& controller,
	    std::uint32_t EngineRenderingSettingsState::* field,
	    std::uint32_t minValue,
	    std::uint32_t maxValue);
	void DrawFloatInputRow(
	    const char* id,
	    const char* label,
	    EngineRenderingSettingsController& controller,
	    float EngineRenderingSettingsState::* field,
	    float step = 1.0f,
	    float stepFast = 10.0f,
	    const char* format = "%.3f");

	template <typename ValueType, std::size_t OptionCount> void DrawComboOptionRow(
	    const char* id,
	    const char* label,
	    EngineRenderingSettingsController& controller,
	    ValueType EngineRenderingSettingsState::* field,
	    const ComboOption<ValueType> (&options)[OptionCount])
	{
		BeginSettingsRow(label);
		const ValueType value = controller.GetState().*field;

		const char* previewLabel = OptionCount > 0 ? options[0].Label : "";
		for (const ComboOption<ValueType>& option : options)
		{
			if (option.Value == value)
			{
				previewLabel = option.Label;
				break;
			}
		}

		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);
		if (ImGui::BeginCombo(id, previewLabel))
		{
			for (const ComboOption<ValueType>& option : options)
			{
				const bool bSelected = option.Value == value;
				if (ImGui::Selectable(option.Label, bSelected))
				{
					controller.Set(field, option.Value);
				}
				if (bSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}
}
