#include "PCH.h"
#include "World/GameWorldState.h"

#include "World/ECS/Components/AnimationComponents.h"
#include "World/ECS/Components/TransformComponents.h"
#include "World/Resources/GameWorldResourceStores.h"

namespace ECS
{
	template <typename T> static std::uint32_t StorageCount(const EntityRegistry& registry) noexcept
	{
		return static_cast<std::uint32_t>(registry.GetStorage<T>().GetEntities().size());
	}

	bool GameWorldState::PrepareSystemExecution(GameWorldResourceStores& resources)
	{
		const bool bAnimationPrepared = m_systemGraph && m_animationOutput.Prepare(m_registry, resources.AnimationClips, resources.Skeletons, m_morphWeights, resources.Generation);
		if (!bAnimationPrepared)
		{
			return false;
		}

		m_extraction.Prepare(GetMeshEntities().size());
		PrepareSystemArena();
		return true;
	}

	void GameWorldState::PrepareSystemArena()
	{
		m_systemArena.CameraChanges.assign(StorageCount<Camera>(m_registry), EntityId::Invalid());
		m_systemArena.AnimationChanges.assign(StorageCount<AnimationState>(m_registry), EntityId::Invalid());
		m_systemArena.MorphChanges.assign(m_animationOutput.GetMorphBindings().size(), EntityId::Invalid());
		m_systemArena.DirtyTransforms.clear();
		if (m_bEvaluateAllTransforms)
		{
			const std::span<const EntityId> entities = m_registry.GetStorage<LocalTransform>().GetEntities();
			m_systemArena.DirtyTransforms.assign(entities.begin(), entities.end());
		}
		else
		{
			m_systemArena.DirtyTransforms.assign(m_dirtyTransforms.begin(), m_dirtyTransforms.end());
		}
		m_systemArena.DirtyTransforms.reserve(m_systemArena.DirtyTransforms.size() + m_systemArena.CameraChanges.size());
		const std::size_t maximumDirtyCount = m_systemArena.DirtyTransforms.size() + m_systemArena.CameraChanges.size();
		m_systemArena.EvaluatedTransforms.assign(maximumDirtyCount, EntityId::Invalid());
		m_systemArena.CameraDerivedChanges.assign(maximumDirtyCount, EntityId::Invalid());
	}
}
