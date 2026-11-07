#pragma once

class EngineRenderingSettingsController;

class RenderingSettingsPanel final
{
public:
	void SetSettings(EngineRenderingSettingsController* settings) noexcept;
	void RefreshFromRuntimeState() noexcept;
	bool HasPendingRestart() const noexcept;
	void BuildUI(bool bDisableInteraction, const char* filterText = nullptr);

private:
	EngineRenderingSettingsController* m_settings = nullptr;
};
