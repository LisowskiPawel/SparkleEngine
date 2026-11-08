#pragma once

struct EngineRenderingSettingsState;

EngineRenderingSettingsState CaptureEngineRenderingSettings() noexcept;
void ApplyEngineRenderingSettings(const EngineRenderingSettingsState& state) noexcept;
