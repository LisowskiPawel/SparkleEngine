#include "PCH.h"

#include "TransformSystemExecution.h"

#include "World/Systems/CameraDerivedStateEvaluationSystem.h"
#include "World/Systems/TransformEvaluationSystem.h"

namespace ECS
{
	TransformSystemExecution::TransformSystemExecution(
	    TransformEvaluationQuery& transforms,
	    CameraDerivedStateQuery& cameras,
	    const std::vector<EntityId>& dirtyTransforms,
	    std::span<EntityId> evaluated,
	    std::span<EntityId> cameraChanges) :
	    m_transformQuery(transforms),
	    m_cameraDerivedQuery(cameras),
	    m_dirtyTransforms(dirtyTransforms),
	    m_evaluated(evaluated),
	    m_cameraChanges(cameraChanges)
	{
		m_transformQuery.PrepareWriteTraversal();
		m_cameraDerivedQuery.PrepareWriteTraversal();
	}

	std::uint32_t TransformSystemExecution::GetDirtyTransformCount() const noexcept
	{
		return static_cast<std::uint32_t>(m_dirtyTransforms.size());
	}

	bool TransformSystemExecution::RunTransforms(std::uint32_t begin, std::uint32_t end)
	{
		return m_transformQuery
		    .ForEachEntityRange(
		        m_dirtyTransforms,
		        begin,
		        end,
		        [this](std::size_t index, EntityId entity, const LocalTransform& local, WorldTransform& world)
		        {
			        TransformEvaluationSystem::Evaluate(local, world);
			        m_evaluated[index] = entity;
		        })
		    .Succeeded();
	}

	bool TransformSystemExecution::RunCameraDerivedState(std::uint32_t begin, std::uint32_t end)
	{
		return m_cameraDerivedQuery
		    .ForEachEntityRange(
		        m_dirtyTransforms,
		        begin,
		        end,
		        [this](std::size_t index, EntityId entity, const LocalTransform& local, const Camera&, CameraDerivedState& derived)
		        {
			        CameraDerivedStateEvaluationSystem::Evaluate(local, derived);
			        m_cameraChanges[index] = entity;
		        })
		    .Succeeded();
	}
}
