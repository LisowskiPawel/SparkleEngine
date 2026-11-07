#pragma once
#include "Icons/EditorIcon.h"

#include <imgui.h>

namespace EditorWidgets
{
	enum class MenuRowKind : std::uint8_t
	{
		Action,
		Toggle,
		Submenu
	};

	enum class MenuCheckState : std::uint8_t
	{
		Hidden,
		Unchecked,
		Checked,
		Mixed
	};

	// Keep this scope around the complete menu hierarchy, including native EndPopup/EndCombo.
	class MenuStyleScope final
	{
	public:
		MenuStyleScope();
		~MenuStyleScope();
		MenuStyleScope(const MenuStyleScope&) = delete;
		MenuStyleScope& operator=(const MenuStyleScope&) = delete;
	};

	float MeasureMenuRow(const char* label, MenuRowKind kind);
	float MeasureMenuSection(const char* label);
	// Pair successful calls with native EndPopup/EndMenu; MenuStyleScope owns style restoration.
	bool BeginMenuPopup(const char* id, ImVec2 anchor, float width);
	bool BeginMenu(const char* label, EditorIcons::EditorIcon icon, float childWidth, bool enabled = true);
	bool DrawMenuItem(
	    const char* label,
	    EditorIcons::EditorIcon icon = EditorIcons::EditorIcon::None,
	    MenuCheckState checkState = MenuCheckState::Hidden,
	    ImGuiSelectableFlags flags = ImGuiSelectableFlags_None);
	void DrawMenuSection(const char* label);
}
