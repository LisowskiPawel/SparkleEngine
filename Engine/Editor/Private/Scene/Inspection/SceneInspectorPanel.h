#pragma once

#include <string>

class EditorSceneSession;

class SceneInspectorPanel final
{
public:
	explicit SceneInspectorPanel(EditorSceneSession& scene, float widthPixels = 560.0f) noexcept;

	float GetWidth() const noexcept { return m_widthPixels; }

	void SetTopInset(float topInsetPixels) noexcept;

	void BuildUI(bool bDisableInteraction = false);

private:
	void BeginWindow() noexcept;
	void BuildDetailsToolbar() noexcept;
	void BuildInspectorTabs() noexcept;
	void DrawSelectionHeader() noexcept;
	void BuildSelectionInspector() noexcept;
	void BuildEditStatus() noexcept;

	EditorSceneSession& m_scene;
	float m_widthPixels = 560.0f;
	float m_topInsetPixels = 0.0f;
	std::string m_filterText;
};
