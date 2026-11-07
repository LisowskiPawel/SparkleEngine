#pragma once
#include "Icons/EditorIcon.h"

namespace EditorWidgets
{
	void DrawEditorIcon(EditorIcons::EditorIcon icon, const char* tooltip = nullptr, bool drawBadgeBackground = true);
	bool DrawEditorIconButton(EditorIcons::EditorIcon icon, const char* id, const char* tooltip = nullptr);
	bool DrawVisibilityIconButton(const char* id, bool visible);
	bool DrawCenteredVisibilityIconButton(const char* id, bool visible) noexcept;
}
