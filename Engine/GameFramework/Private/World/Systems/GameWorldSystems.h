#pragma once

#include "World/Systems/CameraSimulationInput.h"
#include "World/Systems/Execution/AnimationSystemExecution.h"
#include "World/Systems/Execution/MeshExtractionSystemExecution.h"
#include "World/Systems/CameraMovementSystem.h"
#include "World/Systems/Execution/TransformSystemExecution.h"

#include <cstdint>

class GameWorldResourceStores;
struct enkiTaskScheduler;

namespace ECS
{
	class GameWorldState;

	struct GameWorldSystemExecutionContext final
	{
		GameWorldResourceStores& Resources;
		enkiTaskScheduler* Scheduler = nullptr;
		CameraSimulationInput Camera;
	};

	class GameWorldSystemExecution final
	{
	public:
		GameWorldSystemExecution(GameWorldState& state, const GameWorldSystemExecutionContext& context);
		bool Execute(enkiTaskScheduler* scheduler);

	private:
		// Component storage stays fixed until every submitted task settles.
		CameraMovementSystem m_camera;
		AnimationSystemExecution m_animation;
		TransformSystemExecution m_transform;
		MeshExtractionSystemExecution m_extraction;
		GameWorldState& m_state;
	};

}
