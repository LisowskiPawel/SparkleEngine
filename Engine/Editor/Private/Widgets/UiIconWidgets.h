#pragma once
#include "Icons/EditorIcon.h"

namespace EditorWidgets
{
	void DrawEditorIcon(EditorIcons::EditorIcon icon, const char* tooltip = nullptr, bool bDrawBadgeBackground = true);
	bool DrawEditorIconButton(EditorIcons::EditorIcon icon, const char* id, const char* tooltip = nullptr);
	bool DrawVisibilityIconButton(const char* id, bool bVisible);
	bool DrawCenteredVisibilityIconButton(const char* id, bool bVisible) noexcept;
}
