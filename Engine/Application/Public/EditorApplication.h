#pragma once

#include "Application.h"
#include "GameApplication.h"

#include <memory>

class Renderer;

class SPARKLE_APPLICATION_API EditorApplication final : public Application
{
public:
	EditorApplication();
	explicit EditorApplication(GameApplicationOptions options) noexcept;
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
	void UpdateEditorOperations(Renderer& renderer);
	void RenderEditorFrame(Renderer& renderer);

	struct State;
	std::unique_ptr<State> m_state;
	GameApplicationOptions m_gameOptions;
	bool m_isEditorSessionActive = false;
};
