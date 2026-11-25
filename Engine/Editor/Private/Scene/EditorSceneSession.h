#pragma once

#include "Scene/Model/EditorSceneModel.h"
#include "Scene/Transactions/EditorTransactionHistory.h"
#include "World/WorldMaterialVariantView.h"
#include "World/WorldReadView.h"

#include <functional>

struct EditorSceneSource final
{
	std::function<WorldReadView()> AcquireReadView;
	std::function<WorldMaterialVariantView()> MaterialVariants;
};

class EditorSceneSession final
{
public:
	EditorSceneSession(EditorSceneSource source, EditorTransactionHistory::SubmitFunction submit);

	void Refresh();
	void Select(const SceneObjectSelection& selection);
	void ToggleVisibility(const SceneObjectSelection& selection);

	const EditorSceneModel& GetModel() const noexcept { return m_model; }

	const SceneObjectSelection& GetSelection() const noexcept { return m_selection; }

	EditorTransactionHistory& GetHistory() noexcept { return m_history; }

private:
	void SynchronizeSelection(bool bWorldChanged) noexcept;

	EditorSceneSource m_source;
	EditorSceneModel m_model;
	SceneObjectSelection m_selection;
	EditorTransactionHistory m_history;
};
