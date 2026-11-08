#include "PCH.h"

#include "SimulationSystemExecution.h"

namespace ECS
{
	SimulationSystemExecution::SimulationSystemExecution(CameraMovementQuery& query, EntityId activeCamera, const CameraSimulationInput& cameraInput, std::span<EntityId> changes) :
	    m_cameraMovement(activeCamera, cameraInput),
	    m_cameraQuery(query),
	    m_changes(changes)
	{
		m_cameraQuery.PrepareWriteTraversal();
	}

	std::uint32_t SimulationSystemExecution::GetCameraCount() const noexcept
	{
		return static_cast<std::uint32_t>(m_cameraQuery.GetEstimatedEntityCount());
	}

	bool SimulationSystemExecution::RunCamera(std::uint32_t begin, std::uint32_t end)
	{
		return m_cameraQuery
		    .ForEachRange(
		        begin,
		        end,
		        [this](std::size_t index, EntityId entity, Camera& camera, LocalTransform& transform)
		        {
			        if (m_cameraMovement.Apply(entity, camera, transform))
			        {
				        m_changes[index] = entity;
			        }
		        })
		    .Succeeded();
	}
}
