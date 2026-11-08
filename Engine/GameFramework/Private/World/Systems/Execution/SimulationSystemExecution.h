#pragma once

#include "World/Systems/CameraMovementSystem.h"
#include "World/Systems/Descriptors/GameWorldSystemContract.h"

#include <cstdint>
#include <span>

namespace ECS
{
	class SimulationSystemExecution final
	{
	public:
		SimulationSystemExecution(CameraMovementQuery& query, EntityId activeCamera, const CameraSimulationInput& cameraInput, std::span<EntityId> changes);

		std::uint32_t GetCameraCount() const noexcept;
		bool RunCamera(std::uint32_t begin, std::uint32_t end);

	private:
		CameraMovementSystem m_cameraMovement;
		CameraMovementQuery& m_cameraQuery;
		std::span<EntityId> m_changes;
	};
}
