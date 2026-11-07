#pragma once

#include <utility>
#include <memory>
#include <string>

class EditorSceneModel;
class EditorTransactionHistory;
struct SceneObjectSelection;

class SceneInspectorPanel final
{
public:
	SceneInspectorPanel(SceneObjectSelection& selection, EditorTransactionHistory& transactionHistory, float widthPixels = 560.0f) noexcept;
	void SetWidth(float widthPixels) noexcept;

	float GetWidth() const noexcept { return m_widthPixels; }

	void SetTopInset(float topInsetPixels) noexcept;

	void SetModel(std::shared_ptr<const EditorSceneModel> model) noexcept { m_model = std::move(model); }

	void BuildUI(bool disableInteraction = false);

private:
	void BeginWindow() noexcept;
	void BuildDetailsToolbar() noexcept;
	void BuildInspectorTabs() noexcept;
	void BuildSelectionInspector() noexcept;
	void BuildEditStatus() noexcept;

	std::shared_ptr<const EditorSceneModel> m_model;
	EditorTransactionHistory* m_transactionHistory = nullptr;
	SceneObjectSelection* m_selection = nullptr;
	float m_widthPixels = 560.0f;
	float m_topInsetPixels = 0.0f;
	std::string m_filterText;
};
