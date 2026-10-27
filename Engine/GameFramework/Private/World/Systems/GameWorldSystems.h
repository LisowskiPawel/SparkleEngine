#pragma once

#include "World/Systems/CameraSimulationInput.h"
#include "World/Systems/Execution/AnimationSystemExecution.h"
#include "World/Systems/Execution/MeshExtractionSystemExecution.h"
#include "World/Systems/Execution/SimulationSystemExecution.h"
#include "World/Systems/Execution/TransformSystemExecution.h"

#include <cstdint>

class GameWorldResourceStores;
class TaskExecutor;

namespace ECS
{
	class CompiledGameSystemGraph;
	struct GameSystemDesc;
	CompiledGameSystemGraph BuildGameWorldSystemGraph();

	class GameWorldState;

	struct GameWorldSystemExecutionContext final
	{
		GameWorldResourceStores& Resources;
		TaskExecutor& Executor;
		CameraSimulationInput Camera;
	};

	class GameWorldSystemExecution final
	{
	public:
		GameWorldSystemExecution(GameWorldState& state, const GameWorldSystemExecutionContext& context, const StructureFrozenEpoch& epoch);
		static CompiledGameSystemGraph BuildGraph();

	private:
		static GameSystemDesc DescribeCameraMovement();
		static GameSystemDesc DescribeAnimationPlayback();
		static GameSystemDesc DescribeAnimationPoseEvaluation(std::uint32_t playbackIndex);
		static GameSystemDesc DescribeMorphWeightEvaluation(std::uint32_t playbackIndex);
		static GameSystemDesc DescribeSkinningEvaluation(std::uint32_t poseIndex);
		static GameSystemDesc DescribeMorphOutputCommit(std::uint32_t morphIndex);
		static GameSystemDesc DescribeSystemOutputCommit(std::uint32_t skinningIndex, std::uint32_t morphCommitIndex);
		static GameSystemDesc DescribeTransformEvaluation();
		static GameSystemDesc DescribeCameraDerivedState(std::uint32_t transformIndex);
		static GameSystemDesc DescribeMeshExtraction();
		static GameSystemDesc DescribeExtractionCommit(std::uint32_t extractionIndex);

		SimulationSystemExecution m_simulation;
		AnimationSystemExecution m_animation;
		TransformSystemExecution m_transform;
		MeshExtractionSystemExecution m_extraction;
		GameWorldState& m_state;
	};

	bool ExecuteGameWorldSystems(GameWorldState& state, const GameWorldSystemExecutionContext& context);
}
