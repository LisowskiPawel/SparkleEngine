#include "PCH.h"
#include "Scene/EditorSceneSession.h"

#include "Scene/Commands/SceneObjectCommandFactory.h"

#include <optional>
#include <utility>

EditorSceneSession::EditorSceneSession(EditorSceneSource source, EditorTransactionHistory::SubmitFunction submit) :
    m_source(std::move(source)),
    m_history(std::move(submit))
{
	Refresh();
}

void EditorSceneSession::Refresh()
{
	if (!m_source.AcquireReadView)
	{
		return;
	}

	WorldReadView view = m_source.AcquireReadView();
	if (!view.IsValid())
	{
		return;
	}

	const std::uint64_t worldGeneration = view.GetGeneration();
	if (m_model.HasWorld() && m_model.GetWorldGeneration() == worldGeneration)
	{
		if (m_model.GetWorldSequence() == view.GetSequence())
		{
			return;
		}
	}

	WorldMaterialVariantView materialVariants = m_source.MaterialVariants ? m_source.MaterialVariants() : WorldMaterialVariantView{};
	const bool bWorldChanged = m_model.HasWorld() && m_model.GetWorldGeneration() != worldGeneration;
	m_model = EditorSceneModel(std::move(view), std::move(materialVariants));
	m_history.InvalidateForWorldGeneration(worldGeneration);
	SynchronizeSelection(bWorldChanged);
}

void EditorSceneSession::SynchronizeSelection(bool bWorldChanged) noexcept
{
	if (!bWorldChanged && m_model.Contains(m_selection))
	{
		return;
	}

	const auto cameras = m_model.GetCameras();
	m_selection = cameras.empty() ? SceneObjectSelection::None() : SceneObjectSelection::Camera(cameras.front().Entity);
}

void EditorSceneSession::Select(const SceneObjectSelection& selection)
{
	if (!m_model.Contains(selection))
	{
		return;
	}

	if (!(m_selection == selection))
	{
		m_history.FinishInteraction();
	}

	m_selection = selection;

	if (selection.type == SceneObjectType::Camera)
	{
		auto commands = SceneObjectCommandFactory::SetActiveCamera(m_model, selection.entity);
		if (commands)
		{
			(void) m_history.Execute(std::move(commands->Forward), std::move(commands->Inverse), m_model.GetWorldGeneration());
		}
	}
}

void EditorSceneSession::ToggleVisibility(const SceneObjectSelection& selection)
{
	auto commands = SceneObjectCommandFactory::ToggleVisibility(m_model, selection);
	if (commands)
	{
		(void) m_history.Execute(std::move(commands->Forward), std::move(commands->Inverse), m_model.GetWorldGeneration());
	}
}
