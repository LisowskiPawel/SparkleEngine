#pragma once

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

	bool MatchesFilter(const char* filterText, const char* title, const char* keywords);
	bool BeginSettingsCategory(const char* label);
	bool BeginSettingsTable(const char* id);
	void BeginSettingsRow(const char* label);

	bool DrawBooleanRow(const char* id, const char* label, bool& value);
	bool DrawUnsignedIntSliderRow(const char* id, const char* label, std::uint32_t& value, std::uint32_t minValue, std::uint32_t maxValue);
	bool DrawFloatInputRow(const char* id, const char* label, float& value, float step = 1.0f, float stepFast = 10.0f, const char* format = "%.3f");

	template <typename ValueType, std::size_t OptionCount, typename OnChanged>
	void DrawComboOptionRow(const char* id, const char* label, ValueType value, const ComboOption<ValueType> (&options)[OptionCount], OnChanged&& onChanged)
	{
		BeginSettingsRow(label);

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
				const bool selected = option.Value == value;
				if (ImGui::Selectable(option.Label, selected))
				{
					onChanged(option.Value);
				}
				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}
}
