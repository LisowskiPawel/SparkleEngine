#include "PCH.h"
#include "World/GameWorldState.h"

#include "World/ECS/Components/RenderingComponents.h"
#include "World/ECS/Components/TransformComponents.h"

#include "World/Systems/CameraDerivedStateEvaluationSystem.h"
#include "World/Systems/TransformEvaluationSystem.h"

#include <algorithm>

namespace ECS
{
	GameWorldState::GameWorldState()
	{
		m_dirtyTransforms.reserve(MaximumPendingChanges);
		m_pendingChanges.reserve(MaximumPendingChanges);
		CommitDerivedStateAndPublish();
	}

	void GameWorldState::MarkTransformDirty(EntityId entity) noexcept
	{
		if (!entity.IsValid())
		{
			return;
		}
		if (m_dirtyTransforms.size() < m_dirtyTransforms.capacity())
		{
			m_dirtyTransforms.push_back(entity);
		}
		else
		{
			m_bEvaluateAllTransforms = true;
		}
	}

	void GameWorldState::RecordChange(EntityId entity) noexcept
	{
		if (m_bForceFullPublication)
		{
			return;
		}
		if (m_pendingChanges.size() < MaximumPendingChanges)
		{
			m_pendingChanges.push_back(entity);
			return;
		}
		m_pendingChanges.clear();
		m_bForceFullPublication = true;
	}

	void GameWorldState::WriteSkyEnvironment(SkyEnvironment environment) noexcept
	{
		m_skyEnvironment = std::move(environment);
		RecordChange(EntityId::Invalid());
	}

	void GameWorldState::RemoveSkyEnvironment() noexcept
	{
		if (m_skyEnvironment.has_value())
		{
			m_skyEnvironment.reset();
			RecordChange(EntityId::Invalid());
		}
	}

	void GameWorldState::NotifyResourceChanged() noexcept
	{
		RecordChange(EntityId::Invalid());
	}

	void GameWorldState::CommitDerivedStateAndPublish()
	{
		CommitDirtyTransforms(PrepareDirtyTransforms());
		m_dirtyTransforms.clear();
		m_bEvaluateAllTransforms = false;
		PublishPendingChanges();
	}

	std::span<const EntityId> GameWorldState::PrepareDirtyTransforms()
	{
		if (m_bEvaluateAllTransforms)
		{
			return m_registry.GetStorage<LocalTransform>().GetEntities();
		}
		std::sort(m_dirtyTransforms.begin(), m_dirtyTransforms.end());
		m_dirtyTransforms.erase(std::unique(m_dirtyTransforms.begin(), m_dirtyTransforms.end()), m_dirtyTransforms.end());
		return m_dirtyTransforms;
	}

	void GameWorldState::CommitDirtyTransforms(std::span<const EntityId> entities)
	{
		for (EntityId entity : entities)
		{
			if (!m_registry.IsAlive(entity))
			{
				continue;
			}
			if (const LocalTransform* local = m_registry.Get<LocalTransform>(entity))
			{
				WorldTransform world;
				TransformEvaluationSystem::Evaluate(*local, world);
				m_registry.Replace(entity, world);
				if (m_registry.Get<Camera>(entity) != nullptr)
				{
					CameraDerivedState derived;
					CameraDerivedStateEvaluationSystem::Evaluate(*local, derived);
					m_registry.Replace(entity, derived);
				}
			}
		}
	}

	void GameWorldState::PublishPendingChanges()
	{
		if (m_pendingChanges.empty() && !m_bForceFullPublication)
		{
			return;
		}
		std::erase(m_pendingChanges, EntityId::Invalid());
		std::sort(m_pendingChanges.begin(), m_pendingChanges.end());
		m_pendingChanges.erase(std::unique(m_pendingChanges.begin(), m_pendingChanges.end()), m_pendingChanges.end());
		PublishReadView(m_pendingChanges, m_bForceFullPublication);
		m_pendingChanges.clear();
		m_bForceFullPublication = false;
	}

	WorldReadView GameWorldState::AcquireReadView(std::uint64_t generation) const noexcept
	{
		return WorldReadView(m_publishedReadView.load(std::memory_order_acquire), generation);
	}

}
