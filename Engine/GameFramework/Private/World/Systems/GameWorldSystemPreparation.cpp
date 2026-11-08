#include "PCH.h"
#include "World/GameWorldState.h"

#include "World/Resources/GameWorldResourceStores.h"

namespace ECS
{
	bool GameWorldState::PrepareSystemExecution(GameWorldResourceStores& resources)
	{
		const bool bAnimationPrepared = m_animationOutput.Prepare(m_registry, resources.AnimationClips, resources.Skeletons, m_morphWeights, resources.Generation);
		if (!bAnimationPrepared)
		{
			return false;
		}

		m_extraction.Prepare(GetMeshEntities().size());
		return true;
	}

	void GameWorldState::PrepareTransformWork()
	{
		const std::span<const EntityId> dirty = PrepareDirtyTransforms();
		m_transformWork.assign(dirty.begin(), dirty.end());
	}
}
