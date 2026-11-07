#pragma once

#include <string>

namespace EditorWidgets
{
	bool MatchesDetailsFilter(const std::string& filterText, const char* title, const char* keywords) noexcept;

	bool DrawDetailsCategory(const char* title, bool defaultOpen = true);
	void DrawDetailsEmptyState(const char* text = "Select an object from the scene outliner to inspect its properties.");
	void DrawDetailsValueRow(const char* label, const char* value);
	bool EditDetailsFloat(const char* label, float& value, float speed, float minValue, float maxValue, const char* format, const float* resetValue = nullptr);
	bool EditDetailsFloat3(const char* label, float values[3], float speed, float minValue, float maxValue, const char* format, const float* resetValues = nullptr);
	bool EditDetailsColor3(const char* label, float values[3], const float* resetValues = nullptr);
	bool EditDetailsCheckbox(const char* label, bool& value, const bool* resetValue = nullptr);
	bool EditDetailsText(const char* label, std::string& value, const std::string* resetValue = nullptr);
}
