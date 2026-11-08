#include "PCH.h"

#include "World/GameWorldState.h"

#include <algorithm>

namespace ECS
{
	static void SortUnique(std::vector<EntityId>& entities)
	{
		std::sort(entities.begin(), entities.end());
		entities.erase(std::unique(entities.begin(), entities.end()), entities.end());
	}

	void GameWorldState::RecordTransformChanges(std::vector<EntityId>& changes)
	{
		std::erase(changes, EntityId::Invalid());
		SortUnique(changes);
		for (EntityId entity : changes)
		{
			m_systemArena.DirtyTransforms.push_back(entity);
			RecordChange(entity);
		}
	}

	void GameWorldState::RecordSystemChanges(std::vector<EntityId>& changes)
	{
		std::erase(changes, EntityId::Invalid());
		SortUnique(changes);
		for (EntityId entity : changes)
		{
			RecordChange(entity);
		}
	}

	bool GameWorldState::CommitSystemOutputs()
	{
		RecordTransformChanges(m_systemArena.CameraChanges);
		RecordSystemChanges(m_systemArena.AnimationChanges);
		RecordSystemChanges(m_systemArena.MorphChanges);
		SortUnique(m_systemArena.DirtyTransforms);
		return true;
	}

	bool GameWorldState::CommitExtraction()
	{
		m_extraction.CommitMeshes(m_meshInstanceGroups);
		RecordSystemChanges(m_systemArena.EvaluatedTransforms);
		RecordSystemChanges(m_systemArena.CameraDerivedChanges);
		PublishPendingChanges();
		return true;
	}
}
