#pragma once

#include <memory>
#include <functional>
#include <string>

class EngineRenderingSettingsController;
class RenderingSettingsPanel;

class SettingsPanel final
{
public:
	SettingsPanel();
	~SettingsPanel();

	void SetOpen(bool bOpen) noexcept;

	void SetRenderingSettings(EngineRenderingSettingsController* renderingSettings) noexcept;
	void SetRestartHandler(std::function<void()> restartHandler);
	void BuildUI(bool bDisableInteraction);

private:
	bool BeginWindow();
	void DrawRestartBar(float restartBarHeight);
	void DrawToolbar(bool bDisableInteraction);
	bool HasPendingRestart() const noexcept;

	EngineRenderingSettingsController* m_renderingSettings = nullptr;
	std::unique_ptr<RenderingSettingsPanel> m_renderingPanel;
	std::function<void()> m_restartHandler;
	std::string m_filterText;
	bool m_bIsOpen = false;
	bool m_bRefreshFromRuntimeOnNextOpen = true;
};
