#pragma once
#include "Icons/EditorIcon.h"

#include <imgui.h>

namespace EditorWidgets
{
	inline constexpr float EditorIconSize = 14.0f;

	void DrawCenteredGlyph(ImDrawList* drawList, const ImVec2& start, const ImVec2& size, const char* glyph, ImU32 color) noexcept;
	void DrawEditorIcon(EditorIcons::EditorIcon icon, const char* tooltip = nullptr, bool bDrawBadgeBackground = true);
	bool DrawEditorIconButton(EditorIcons::EditorIcon icon, const char* id, const char* tooltip = nullptr);
	bool DrawVisibilityIconButton(const char* id, bool bVisible);
	bool DrawCenteredVisibilityIconButton(const char* id, bool bVisible) noexcept;
}
