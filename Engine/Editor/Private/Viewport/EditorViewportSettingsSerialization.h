#pragma once

#include <iosfwd>

struct EditorViewportSettingsState;

namespace EditorViewportSettingsSerialization
{
	void Read(std::istream& input, EditorViewportSettingsState& state);
	void Write(std::ostream& output, const EditorViewportSettingsState& state);
}
