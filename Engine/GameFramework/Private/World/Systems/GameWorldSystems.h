#pragma once

#include "World/Systems/CameraSimulationInput.h"
#include "World/Systems/Execution/AnimationSystemExecution.h"
#include "World/Systems/Execution/MeshExtractionSystemExecution.h"
#include "World/Systems/CameraMovementSystem.h"
#include "World/Systems/Execution/TransformSystemExecution.h"
#include "Tasks/Public/TaskGraph.h"

#include <cstdint>

class GameWorldResourceStores;
class TaskExecutor;

namespace ECS
{
	class GameWorldState;

	struct GameWorldSystemTasks final
	{
		struct UpdateCounts final
		{
			std::uint32_t Playback = 0;
			std::uint32_t Poses = 0;
			std::uint32_t MorphSamples = 0;

			bool operator==(const UpdateCounts&) const noexcept = default;
		};

		UpdateCounts Counts;
		CompiledTaskGraph Update;
		std::uint32_t DirtyTransformCount = 0;
		CompiledTaskGraph Transforms;
		std::uint32_t MeshCount = 0;
		CompiledTaskGraph Extraction;
	};

	struct GameWorldSystemExecutionContext final
	{
		GameWorldResourceStores& Resources;
		TaskExecutor& Executor;
		CameraSimulationInput Camera;
	};

	class GameWorldSystemExecution final
	{
	public:
		GameWorldSystemExecution(GameWorldState& state, const GameWorldSystemExecutionContext& context);
		bool Execute(TaskExecutor& executor);

	private:
		static CompiledTaskGraph BuildUpdateTasks(const GameWorldSystemTasks::UpdateCounts& counts);
		static CompiledTaskGraph BuildTransformTasks(std::uint32_t count);
		static CompiledTaskGraph BuildExtractionTasks(std::uint32_t count);

		// Component storage stays fixed until every submitted task settles.
		CameraMovementSystem m_camera;
		AnimationSystemExecution m_animation;
		TransformSystemExecution m_transform;
		MeshExtractionSystemExecution m_extraction;
		GameWorldState& m_state;
	};

}
