#pragma once

class EngineRenderingSettingsController;
struct EngineRenderingSettingsState;
struct ViewportExposureOverrides;

namespace ExposureSettingsEditor
{
	void DrawSettings(EngineRenderingSettingsController& settingsController, const EngineRenderingSettingsState& settings);
	bool DrawOverrides(ViewportExposureOverrides& overrides, const EngineRenderingSettingsState& defaults) noexcept;
}
