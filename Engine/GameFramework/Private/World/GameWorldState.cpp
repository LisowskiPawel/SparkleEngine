#include "PCH.h"
#include "World/GameWorldState.h"

#include "World/ECS/Components/RenderingComponents.h"
#include "World/ECS/Components/TransformComponents.h"

#include "World/Systems/CameraDerivedStateEvaluationSystem.h"
#include "World/Systems/GameWorldSystems.h"
#include "World/Systems/TransformEvaluationSystem.h"

#include <algorithm>
#include <stdexcept>

namespace ECS
{
	GameWorldState::GameWorldState() :
	    m_systemGraph(BuildGameWorldSystemGraph())
	{
		if (!m_systemGraph)
		{
			throw std::runtime_error(m_systemGraph.GetError().Message);
		}
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
			m_evaluateAllTransforms = true;
		}
	}

	void GameWorldState::RecordChange(EntityId entity) noexcept
	{
		if (m_changesOverflowed)
		{
			return;
		}
		if (m_pendingChanges.size() < MaximumPendingChanges)
		{
			m_pendingChanges.push_back(entity);
			return;
		}
		m_pendingChanges.clear();
		m_pendingChanges.push_back(EntityId::Invalid());
		m_forceFullPublication = true;
		m_changesOverflowed = true;
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
		m_evaluateAllTransforms = false;
		PublishPendingChanges();
	}

	std::span<const EntityId> GameWorldState::PrepareDirtyTransforms()
	{
		if (m_evaluateAllTransforms)
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
			RecordChange(entity);
		}
	}

	void GameWorldState::PublishPendingChanges()
	{
		if (m_pendingChanges.empty() && m_publishedReadView.load(std::memory_order_acquire) != nullptr)
		{
			return;
		}
		m_readSequence += static_cast<WorldSequence>(m_pendingChanges.size());
		PublishReadView(m_pendingChanges, m_readSequence, m_forceFullPublication);
		m_pendingChanges.clear();
		m_changesOverflowed = false;
		m_forceFullPublication = false;
	}

	WorldReadView GameWorldState::AcquireReadView() const noexcept
	{
		return WorldReadView(m_publishedReadView.load(std::memory_order_acquire));
	}

}
