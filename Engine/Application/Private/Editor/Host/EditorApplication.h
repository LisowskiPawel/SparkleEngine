#pragma once

#include "Host/Application.h"
#include "EditorShaderCompileAction.h"

#include <memory>

class Renderer;
class GameApplication;
class UI;
class TaskScope;
class EditorShaderCompileOperation;
class EditorViewportOutputCoordinator;
struct RenderViewCameraData;

class EditorApplication final : public Application
{
public:
	explicit EditorApplication(EditorShaderCompileAction compileShaders = {}) noexcept;
	~EditorApplication();

	EditorApplication(const EditorApplication&) = delete;
	EditorApplication& operator=(const EditorApplication&) = delete;
	EditorApplication(EditorApplication&&) = delete;
	EditorApplication& operator=(EditorApplication&&) = delete;

	void Initialize() override;
	bool Tick() override;
	void Shutdown() override;

private:
	void InitializeGameApplication();
	void InitializeEditorOperations();
	void InitializeUi();
	void ConfigureUiDiagnostics(Renderer& renderer);
	void Restart();
	RenderViewCameraData UpdateViewportCamera();
	void ReloadShaders(Renderer& renderer);
	void UpdateEditorOperations(Renderer& renderer);
	void RenderEditorFrame(Renderer& renderer);

	EditorShaderCompileAction m_compileShaders;
	std::unique_ptr<GameApplication> m_game;
	std::unique_ptr<TaskScope> m_operationScope;
	std::unique_ptr<EditorViewportOutputCoordinator> m_viewportOutput;
	std::unique_ptr<EditorShaderCompileOperation> m_shaderCompileOperation;
	std::unique_ptr<UI> m_ui;
	bool m_bIsEditorSessionActive = false;
};
