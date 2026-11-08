#include "PCH.h"

#include "TransformSystemExecution.h"

#include "World/Systems/CameraDerivedStateEvaluationSystem.h"
#include "World/Systems/TransformEvaluationSystem.h"
#include "World/ECS/EntityRegistry.h"

namespace ECS
{
	TransformSystemExecution::TransformSystemExecution(EntityRegistry& registry, const std::vector<EntityId>& dirtyTransforms) :
	    m_registry(registry),
	    m_dirtyTransforms(dirtyTransforms)
	{
	}

	std::uint32_t TransformSystemExecution::GetDirtyTransformCount() const noexcept
	{
		return static_cast<std::uint32_t>(m_dirtyTransforms.size());
	}

	bool TransformSystemExecution::RunTransforms(std::uint32_t begin, std::uint32_t end)
	{
		if (begin > end || end > m_dirtyTransforms.size())
		{
			return false;
		}
		for (std::uint32_t index = begin; index < end; ++index)
		{
			const EntityId entity = m_dirtyTransforms[index];
			const LocalTransform* local = m_registry.Get<LocalTransform>(entity);
			WorldTransform* world = m_registry.GetMutable<WorldTransform>(entity);
			if (local != nullptr && world != nullptr)
			{
				TransformEvaluationSystem::Evaluate(*local, *world);
			}
		}
		return true;
	}

	bool TransformSystemExecution::RunCameraDerivedState(std::uint32_t begin, std::uint32_t end)
	{
		if (begin > end || end > m_dirtyTransforms.size())
		{
			return false;
		}
		for (std::uint32_t index = begin; index < end; ++index)
		{
			const EntityId entity = m_dirtyTransforms[index];
			if (m_registry.Get<Camera>(entity) == nullptr)
			{
				continue;
			}
			const LocalTransform* local = m_registry.Get<LocalTransform>(entity);
			CameraDerivedState* derived = m_registry.GetMutable<CameraDerivedState>(entity);
			if (local != nullptr && derived != nullptr)
			{
				CameraDerivedStateEvaluationSystem::Evaluate(*local, *derived);
			}
		}
		return true;
	}
}
