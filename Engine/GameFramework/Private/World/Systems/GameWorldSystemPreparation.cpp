#include "PCH.h"
#include "World/GameWorldState.h"

#include "World/ECS/Components/AnimationComponents.h"
#include "World/ECS/Components/TransformComponents.h"
#include "World/Resources/GameWorldResourceStores.h"

namespace ECS
{
	template <typename T> static std::uint32_t StorageCount(const EntityRegistry& registry) noexcept
	{
		const ComponentStorage<T>* storage = registry.FindStorage<T>();
		return storage == nullptr ? 0u : static_cast<std::uint32_t>(storage->GetEntities().size());
	}

	bool GameWorldState::PrepareSystemExecution(GameWorldResourceStores& resources)
	{
		if (!m_systemGraph
		    || !m_animationOutput.Prepare(m_registry, resources.AnimationClips, resources.Skeletons, m_morphWeights, resources.Generation)
		    || !m_extraction.Prepare(m_registry))
		{
			return false;
		}

		PrepareSystemArena();
		return true;
	}

	void GameWorldState::PrepareSystemArena()
	{
		m_systemArena.CameraChanges.assign(StorageCount<Camera>(m_registry), EntityId::Invalid());
		m_systemArena.AnimationChanges.assign(StorageCount<AnimationState>(m_registry), EntityId::Invalid());
		m_systemArena.MorphChanges.assign(m_animationOutput.GetMorphBindings().size(), EntityId::Invalid());
		m_systemArena.DirtyTransforms.clear();
		if (m_evaluateAllTransforms)
		{
			const ComponentStorage<LocalTransform>* transforms = m_registry.FindStorage<LocalTransform>();
			if (transforms != nullptr)
			{
				m_systemArena.DirtyTransforms.assign(transforms->GetEntities().begin(), transforms->GetEntities().end());
			}
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
