#include "PCH.h"

#include "World/Systems/GameWorldSystems.h"

#include "World/GameWorldState.h"
#include "World/Resources/GameWorldResourceStores.h"
#include "World/Systems/GameSystemGraph.h"

namespace ECS
{


	GameWorldSystemExecution::GameWorldSystemExecution(GameWorldState& state, const GameWorldSystemExecutionContext& context, const StructureFrozenEpoch& epoch) :
	    m_simulation(state, context.Camera, epoch),
	    m_animation(state, context.Resources, context.Camera.DeltaSeconds, epoch),
	    m_transform(state, epoch),
	    m_extraction(state, context.Resources.Skeletons, epoch),
	    m_state(state)
	{
	}

	CompiledGameSystemGraph GameWorldSystemExecution::BuildGraph()
	{
		GameSystemGraph graph;
		graph.Add(DescribeCameraMovement());
		const auto playbackIndex = graph.Add(DescribeAnimationPlayback());
		const auto poseIndex = graph.Add(DescribeAnimationPoseEvaluation(playbackIndex));
		const auto morphIndex = graph.Add(DescribeMorphWeightEvaluation(playbackIndex));
		const auto skinningIndex = graph.Add(DescribeSkinningEvaluation(poseIndex));
		const auto morphCommitIndex = graph.Add(DescribeMorphOutputCommit(morphIndex));
		graph.Add(DescribeSystemOutputCommit(skinningIndex, morphCommitIndex));
		const auto transformIndex = graph.Add(DescribeTransformEvaluation());
		graph.Add(DescribeCameraDerivedState(transformIndex));
		const auto extractionIndex = graph.Add(DescribeMeshExtraction());
		graph.Add(DescribeExtractionCommit(extractionIndex));
		return graph.Compile();
	}

	CompiledGameSystemGraph BuildGameWorldSystemGraph()
	{
		return GameWorldSystemExecution::BuildGraph();
	}

	bool ExecuteGameWorldSystems(GameWorldState& state, const GameWorldSystemExecutionContext& context)
	{
		if (!state.PrepareSystemExecution(context.Resources))
		{
			return false;
		}

		StructureFrozenEpoch epoch = state.m_registry.FreezeStructure();
		if (!epoch.IsValid())
		{
			return false;
		}
		GameWorldSystemExecution systems(state, context, epoch);
		GameSystemGraphError error;
		const bool executed = state.m_systemGraph.Execute(context.Executor, systems, error);
		if (executed)
		{
			state.m_dirtyTransforms.clear();
			state.m_evaluateAllTransforms = false;
		}
		return executed;
	}
}
