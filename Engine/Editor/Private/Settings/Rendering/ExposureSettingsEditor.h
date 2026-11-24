#pragma once

class EngineRenderingSettingsController;
struct EngineRenderingSettingsState;
struct ViewportExposureOverrides;

namespace ExposureSettingsEditor
{
	void DrawSettings(EngineRenderingSettingsController& settingsController);
	bool DrawOverrides(ViewportExposureOverrides& overrides, const EngineRenderingSettingsState& defaults) noexcept;
}
