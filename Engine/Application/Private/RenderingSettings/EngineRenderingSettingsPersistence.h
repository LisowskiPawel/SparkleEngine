#pragma once

struct EngineRenderingSettingsState;

namespace EngineRenderingSettingsPersistence
{
	void Apply();
	void Write(const EngineRenderingSettingsState& state);
}
