#pragma once

struct EngineRenderingSettingsState;

class EngineRenderingSettingsControls final
{
public:
	static EngineRenderingSettingsState Capture() noexcept;
	static void Apply(const EngineRenderingSettingsState& state) noexcept;
};
