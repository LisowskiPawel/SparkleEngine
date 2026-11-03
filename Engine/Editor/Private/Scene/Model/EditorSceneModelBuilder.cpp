#include "PCH.h"
#include "Scene/Model/EditorSceneModelBuilder.h"

#include <memory>
#include <utility>

std::shared_ptr<const EditorSceneModel> EditorSceneModelBuilder::Update()
{
	if (!m_source.AcquireReadView || !m_source.WorldGeneration)
	{
		return m_current;
	}
	WorldReadView view = m_source.AcquireReadView();
	if (!view.IsValid())
	{
		return m_current;
	}
	const std::uint64_t worldGeneration = m_source.WorldGeneration();
	if (m_current && m_current->GetWorldGeneration() == worldGeneration && m_current->GetWorldSequence() == view.GetSequence())
	{
		return m_current;
	}
	auto model = std::make_shared<EditorSceneModel>();
	model->m_worldGeneration = worldGeneration;
	model->m_readView = std::move(view);
	if (m_source.MaterialVariants)
	{
		model->m_materialVariants = m_source.MaterialVariants();
	}
	model->RebuildEntries();
	m_current = std::move(model);
	return m_current;
}
