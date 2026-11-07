#pragma once

namespace EditorWidgets
{
	bool DrawFilterChip(const char* label, bool active) noexcept;
	void DrawMutedText(const char* text, float alpha = 0.72f) noexcept;
	void DrawPanelHeader(const char* title, const char* subtitle);
	void DrawKeyValueRow(const char* label, const char* value);
	void DrawSectionHeader(const char* title);
}
