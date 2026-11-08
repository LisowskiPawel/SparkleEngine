#include "PCH.h"

#include "World/Systems/GameWorldSystems.h"

#include "World/GameWorldState.h"
#include "World/ECS/StructureFrozenEpoch.h"
#include "World/Resources/GameWorldResourceStores.h"
#include "World/Systems/GameSystemGraph.h"

namespace ECS
{

	GameWorldSystemExecution::GameWorldSystemExecution(GameWorldState& state, const GameWorldSystemExecutionContext& context, const StructureFrozenEpoch& epoch) :
	    m_cameraQuery(state.m_registry, epoch),
	    m_playbackQuery(state.m_registry, epoch),
	    m_poseQuery(state.m_registry, epoch),
	    m_morphQuery(state.m_registry, epoch),
	    m_transformQuery(state.m_registry, epoch),
	    m_cameraDerivedQuery(state.m_registry, epoch),
	    m_meshQuery(state.m_registry, epoch),
	    m_simulation(m_cameraQuery, state.m_activeCamera, context.Camera, state.m_systemArena.CameraChanges),
	    m_animation(
	        m_playbackQuery,
	        m_poseQuery,
	        m_morphQuery,
	        context.Resources.AnimationClips,
	        context.Resources.Skeletons,
	        state.m_animationOutput,
	        state.m_morphWeights,
	        state.m_systemArena.AnimationChanges,
	        state.m_systemArena.MorphChanges,
	        context.Camera.DeltaSeconds),
	    m_transform(m_transformQuery, m_cameraDerivedQuery, state.m_systemArena.DirtyTransforms, state.m_systemArena.EvaluatedTransforms, state.m_systemArena.CameraDerivedChanges),
	    m_extraction(m_meshQuery, state.m_meshResources, context.Resources.Skeletons, state.m_extraction.GetMeshSlots()),
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

	bool GameWorldState::ExecuteSystems(const GameWorldSystemExecutionContext& context)
	{
		if (!PrepareSystemExecution(context.Resources))
		{
			return false;
		}

		StructureFrozenEpoch epoch = m_registry.FreezeStructure();
		if (!epoch.IsValid())
		{
			return false;
		}
		GameWorldSystemExecution systems(*this, context, epoch);
		GameSystemGraphError error;
		const bool bExecuted = m_systemGraph.Execute(context.Executor, systems, error);
		if (bExecuted)
		{
			m_dirtyTransforms.clear();
			m_bEvaluateAllTransforms = false;
		}
		return bExecuted;
	}
}
