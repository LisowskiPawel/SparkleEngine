#pragma once

#include <functional>

class LevelSession;
class Window;

class MainMenuBarPanel final
{
public:
	MainMenuBarPanel(LevelSession* levelSession = nullptr, Window* window = nullptr) noexcept;
	~MainMenuBarPanel() = default;

	MainMenuBarPanel(const MainMenuBarPanel&) = delete;
	MainMenuBarPanel(MainMenuBarPanel&&) = delete;
	MainMenuBarPanel& operator=(const MainMenuBarPanel&) = delete;
	MainMenuBarPanel& operator=(MainMenuBarPanel&&) = delete;

	void SetLevelSession(LevelSession* levelSession) noexcept;
	void SetWindow(Window* window) noexcept;
	void SetShaderCompileHandler(std::function<void(bool)> handler);
	void SetShaderReloadHandler(std::function<void()> handler);
	void SetSettingsOpenHandler(std::function<void()> handler);
	void SetConsoleOpenHandler(std::function<void()> handler);
	void SetViewportCaptureHandler(std::function<void()> handler);
	void BuildUI();
	void BuildOverlays() noexcept;

	float GetHeight() const noexcept { return m_heightPixels; }

private:
	void BuildShaderMenu();
	void BuildFileMenu();
	void BuildWindowsMenu() noexcept;
	void BuildHelpWindow() noexcept;
	void BuildLevelStatus() noexcept;
	void BuildOpenLevelMenu() noexcept;

	LevelSession* m_levelSession = nullptr;
	Window* m_window = nullptr;
	std::function<void(bool)> m_shaderCompileHandler;
	std::function<void()> m_shaderReloadHandler;
	std::function<void()> m_settingsOpenHandler;
	std::function<void()> m_consoleOpenHandler;
	std::function<void()> m_viewportCaptureHandler;
	bool m_bHelpOpen = false;
	bool m_bLevelFailureVisible = false;
	float m_heightPixels = 0.0f;
};
