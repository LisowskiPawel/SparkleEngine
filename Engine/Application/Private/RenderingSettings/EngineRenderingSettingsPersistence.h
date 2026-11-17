#pragma once

#include "ApplicationAPI.h"

struct EngineRenderingSettingsState;

namespace EngineRenderingSettingsPersistence
{
	void Apply();
	SPARKLE_APPLICATION_API void Write(const EngineRenderingSettingsState& state);
}
