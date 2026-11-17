#pragma once

#include "UI.h"
#include "Scene/EditorSceneSession.h"

class EditorIconService;
class EditorImGuiContext;
class Timer;
class MainMenuBarPanel;
class EditorConsoleSystem;
class SceneOutlinerPanel;
class SceneInspectorPanel;
class ViewportPanel;
class ViewportToolbar;
class EditorViewportSession;
class SettingsPanel;
class ShaderToolsPanel;
class UsedMeshesPanel;
class UsedTexturesPanel;
class EngineRenderingSettingsController;
class InputSystem;
class LevelSession;
class Window;
class ImGuiRenderPacketBuilder;

class UI::Implementation final
{
public:
	explicit Implementation(EditorHostServices hostServices);
	~Implementation() noexcept;

	const ViewportRenderRequest& GetViewportRenderRequest() const noexcept;
	RenderViewCameraData UpdateViewportCamera(const CameraInputIntent& intent, float deltaSeconds);
	void SetViewportOverlay(std::unique_ptr<ViewportOverlay> overlay) noexcept;
	void SetViewportRenderProducts(const ViewportRenderProducts& products) noexcept;
	void SetViewportFinalColorTexture(UiTextureHandle texture) noexcept;
	void SetDiagnosticsProviders(EditorDiagnosticsProviders providers);
	void SetShaderCompileHandler(std::function<ConsoleCommandResult(bool)> handler);
	void SetShaderCompileStatus(bool bBusy, std::string_view message);
	RendererMemoryDiagnosticsSnapshot CaptureMemoryDiagnostics() const;

	void AppendConsoleOutput(ConsoleOutputRecord record, bool bShowConsole);

	bool ConsumeShaderReloadRequest() noexcept;
	ViewportOutputAction ConsumeViewportOutputAction() noexcept;
	UiRenderPacket ConsumeRenderPacket();

	void Update();

private:
	void NewFrame();

	void Build();
	void BeginInputRouting(bool bDisableInteraction);
	float BuildMainMenuBar();
	void BuildSceneOutliner(bool bDisableInteraction, float mainMenuBarHeight);
	void BuildCenterWorkspace(bool bDisableInteraction, float mainMenuBarHeight);
	void BuildViewport(bool bDisableInteraction, float topInset, float bottomInset, float outlinerWidth, float inspectorWidth);
	void RegisterViewportInputRegion();
	void BuildSceneInspector(bool bDisableInteraction, float mainMenuBarHeight);
	void BuildUtilityPanels(bool bDisableInteraction);

	void InitializeImGuiContext();
	void InitializeRenderingSettings(EditorHostServices& hostServices);
	void InitializeConsole(CVarControlExecutor&& consoleVariables);

	void InitializeWorkspace(std::function<void()> requestRestart);
	void InitializeCorePanels(std::function<void()> requestRestart);
	void InitializeViewport();
	void InitializeAssetPanels();
	void InitializeScenePanels();
	void HandleTransactionShortcuts(bool bDisableInteraction);
	void ConfigureMainMenuBarWindowActions();

	void SubscribeToWindowEvents();

	std::unique_ptr<EditorImGuiContext> m_imguiContext;
	CVarControlExecutor m_consoleVariables;
	std::unique_ptr<EditorIconService> m_icons;
	EditorSceneSession m_scene;
	std::unique_ptr<EngineRenderingSettingsController> m_renderingSettings;
	std::unique_ptr<EditorViewportSession> m_viewportSession;
	std::unique_ptr<MainMenuBarPanel> m_mainMenuBar;
	std::unique_ptr<EditorConsoleSystem> m_editorConsoleSystem;
	std::unique_ptr<SceneOutlinerPanel> m_sceneOutlinerPanel;
	std::unique_ptr<SceneInspectorPanel> m_sceneInspectorPanel;
	std::unique_ptr<ViewportToolbar> m_viewportToolbar;
	std::unique_ptr<ViewportPanel> m_viewportPanel;
	std::unique_ptr<SettingsPanel> m_settingsPanel;
	std::unique_ptr<ShaderToolsPanel> m_shaderToolsPanel;
	std::unique_ptr<UsedMeshesPanel> m_usedMeshesPanel;
	std::unique_ptr<UsedTexturesPanel> m_usedTexturesPanel;
	Timer& m_timer;
	LevelSession* m_levelSession = nullptr;
	Window& m_window;
	InputSystem& m_inputSystem;
	std::function<RendererMemoryDiagnosticsSnapshot()> m_memoryDiagnosticsProvider;
	std::unique_ptr<ImGuiRenderPacketBuilder> m_renderPacketBuilder;
	UiRenderPacket m_renderPacket;
	std::uint64_t m_viewportGeneration = 0;
	bool m_bShaderReloadRequested = false;
};
