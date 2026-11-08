#include "PCH.h"

#include "World/Systems/GameWorldSystems.h"

#include "World/GameWorldState.h"
#include "World/ECS/EntityIterationScope.h"
#include "World/Resources/GameWorldResourceStores.h"
#include "Tasks/Public/TaskExecutionContext.h"
#include "Tasks/Public/TaskExecutor.h"
#include "Tasks/Public/ParallelFor.h"

#include <string_view>

namespace ECS
{
	namespace GameWorldSystemGrain
	{
		constexpr ParallelForPolicy Animation{.GrainSize = 8, .SerialThreshold = 16, .MaximumPartitions = 16};
		constexpr ParallelForPolicy Pose{.GrainSize = 2, .SerialThreshold = 4, .MaximumPartitions = 16};
		constexpr ParallelForPolicy Transform{.GrainSize = 64, .SerialThreshold = 128, .MaximumPartitions = 16};
		constexpr ParallelForPolicy Extraction{.GrainSize = 64, .SerialThreshold = 128, .MaximumPartitions = 16};
	}

	template <typename Execute> static TaskResult ExecuteSystemTask(TaskExecutionContext& context, Execute execute)
	{
		if (context.IsCancellationRequested())
		{
			return TaskResult::Cancelled("World update cancelled at the owner boundary.");
		}
		auto* systems = context.TryGet<GameWorldSystemExecution>();
		return systems != nullptr && execute(*systems) ? TaskResult::Success() : TaskResult::Failure("World update failed to evaluate its assigned work.");
	}

	template <typename Execute> static TaskNodeHandle AddSystemRange(TaskGraphBuilder& tasks, std::string_view name, std::uint32_t count, ParallelForPolicy policy, Execute execute)
	{
		return ParallelFor(
		    tasks,
		    TaskDesc{TaskName(name), TaskLane::FrameCritical},
		    count,
		    policy,
		    [execute](std::uint32_t begin, std::uint32_t end, TaskExecutionContext& context)
		    {
			    if (begin == end)
			    {
				    return TaskResult::Success();
			    }
			    return ExecuteSystemTask(context, [=](GameWorldSystemExecution& systems) { return execute(systems, begin, end); });
		    });
	}

	static bool ExecuteSystemTasks(const CompiledTaskGraph& tasks, TaskExecutor& executor, GameWorldSystemExecution& systems)
	{
		if (!tasks.IsValid())
		{
			return false;
		}
		TaskExecutionContext context(systems);
		TaskExecution execution = executor.Submit(tasks, context);
		return execution.IsValid() && execution.GetStatus() == TaskExecutionStatus::Succeeded;
	}

	GameWorldSystemExecution::GameWorldSystemExecution(GameWorldState& state, const GameWorldSystemExecutionContext& context) :
	    m_camera(state.m_registry, state.m_activeCamera, context.Camera),
	    m_animation(state.m_registry, context.Resources.AnimationClips, context.Resources.Skeletons, state.m_animationOutput, state.m_morphWeights, context.Camera.DeltaSeconds),
	    m_transform(state.m_registry, state.m_transformWork),
	    m_extraction(state.m_registry, state.m_meshResources, context.Resources.Skeletons, state.m_extraction.GetMeshSlots()),
	    m_state(state)
	{
	}

	CompiledTaskGraph GameWorldSystemExecution::BuildUpdateTasks(const GameWorldSystemTasks::UpdateCounts& counts)
	{
		TaskGraphBuilder tasks;

		const auto camera = tasks.Add(
		    TaskDesc{TaskName("Game.CameraMovement"), TaskLane::FrameCritical},
		    [](TaskExecutionContext& context)
		    {
			    return ExecuteSystemTask(
			        context,
			        [](GameWorldSystemExecution& systems)
			        {
				        const EntityId changed = systems.m_camera.Update();
				        if (changed.IsValid())
				        {
					        systems.m_state.MarkTransformDirty(changed);
					        systems.m_state.RecordChange(changed);
				        }
				        return true;
			        });
		    });

		const auto playback = AddSystemRange(
		    tasks,
		    "Game.AnimationPlaybackAdvance",
		    counts.Playback,
		    GameWorldSystemGrain::Animation,
		    [](GameWorldSystemExecution& systems, std::uint32_t begin, std::uint32_t end) { return systems.m_animation.RunPlayback(begin, end); });

		const auto pose = AddSystemRange(
		    tasks,
		    "Game.AnimationPoseEvaluation",
		    counts.Poses,
		    GameWorldSystemGrain::Pose,
		    [](GameWorldSystemExecution& systems, std::uint32_t begin, std::uint32_t end) { return systems.m_animation.RunPose(begin, end); });

		const auto morph = AddSystemRange(
		    tasks,
		    "Game.MorphWeightEvaluation",
		    counts.MorphSamples,
		    GameWorldSystemGrain::Animation,
		    [](GameWorldSystemExecution& systems, std::uint32_t begin, std::uint32_t end) { return systems.m_animation.RunMorphSamples(begin, end); });

		const auto skinning = AddSystemRange(
		    tasks,
		    "Game.SkinningMatrixEvaluation",
		    counts.Poses,
		    GameWorldSystemGrain::Pose,
		    [](GameWorldSystemExecution& systems, std::uint32_t begin, std::uint32_t end) { return systems.m_animation.RunSkinning(begin, end); });

		const auto morphCommit = tasks.Add(
		    TaskDesc{TaskName("Game.MorphOutputCommit"), TaskLane::FrameCritical},
		    [](TaskExecutionContext& context) { return ExecuteSystemTask(context, [](GameWorldSystemExecution& systems) { return systems.m_animation.CommitMorphOutputs(); }); });

		const auto transformPreparation = tasks.Add(
		    TaskDesc{TaskName("Game.PrepareTransforms"), TaskLane::FrameCritical},
		    [](TaskExecutionContext& context)
		    {
			    return ExecuteSystemTask(
			        context,
			        [](GameWorldSystemExecution& systems)
			        {
				        systems.m_state.PrepareTransformWork();
				        return true;
			        });
		    });

		tasks.DependsOn(playback, camera);
		tasks.DependsOn(pose, playback);
		tasks.DependsOn(morph, playback);
		tasks.DependsOn(skinning, pose);
		tasks.DependsOn(skinning, morph);
		tasks.DependsOn(morphCommit, pose);
		tasks.DependsOn(morphCommit, morph);
		tasks.DependsOn(transformPreparation, skinning);
		tasks.DependsOn(transformPreparation, morphCommit);
		return tasks.Compile();
	}

	CompiledTaskGraph GameWorldSystemExecution::BuildTransformTasks(std::uint32_t count)
	{
		TaskGraphBuilder tasks;

		const auto transforms = AddSystemRange(
		    tasks,
		    "Game.TransformEvaluation",
		    count,
		    GameWorldSystemGrain::Transform,
		    [](GameWorldSystemExecution& systems, std::uint32_t begin, std::uint32_t end) { return systems.m_transform.RunTransforms(begin, end); });

		const auto cameras = AddSystemRange(
		    tasks,
		    "Game.CameraDerivedState",
		    count,
		    GameWorldSystemGrain::Transform,
		    [](GameWorldSystemExecution& systems, std::uint32_t begin, std::uint32_t end) { return systems.m_transform.RunCameraDerivedState(begin, end); });

		tasks.DependsOn(cameras, transforms);
		return tasks.Compile();
	}

	CompiledTaskGraph GameWorldSystemExecution::BuildExtractionTasks(std::uint32_t count)
	{
		TaskGraphBuilder tasks;

		const auto meshes = AddSystemRange(
		    tasks,
		    "Game.MeshExtraction",
		    count,
		    GameWorldSystemGrain::Extraction,
		    [](GameWorldSystemExecution& systems, std::uint32_t begin, std::uint32_t end) { return systems.m_extraction.Run(begin, end); });

		const auto commit = tasks.Add(
		    TaskDesc{TaskName("Game.ExtractionCommit"), TaskLane::FrameCritical},
		    [](TaskExecutionContext& context)
		    {
			    return ExecuteSystemTask(
			        context,
			        [](GameWorldSystemExecution& systems)
			        {
				        systems.m_state.CommitExtraction();
				        return true;
			        });
		    });

		tasks.DependsOn(commit, meshes);
		return tasks.Compile();
	}

	bool GameWorldSystemExecution::Execute(TaskExecutor& executor)
	{
		if (!m_state.m_registry.IsStructureFrozen())
		{
			return false;
		}
		GameWorldSystemTasks& tasks = m_state.m_systemTasks;

		const GameWorldSystemTasks::UpdateCounts counts{.Playback = m_animation.GetPlaybackCount(), .Poses = m_animation.GetPoseCount(), .MorphSamples = m_animation.GetMorphSampleCount()};

		if (!tasks.Update.IsValid() || counts != tasks.Counts)
		{
			tasks.Update = BuildUpdateTasks(counts);
			tasks.Counts = counts;
		}
		if (!ExecuteSystemTasks(tasks.Update, executor, *this))
		{
			return false;
		}

		// Read the prepared ranges after camera movement and animation settle.
		const auto dirtyCount = m_transform.GetDirtyTransformCount();
		if (!tasks.Transforms.IsValid() || dirtyCount != tasks.DirtyTransformCount)
		{
			tasks.Transforms = BuildTransformTasks(dirtyCount);
			tasks.DirtyTransformCount = dirtyCount;
		}
		if (!ExecuteSystemTasks(tasks.Transforms, executor, *this))
		{
			return false;
		}

		const auto meshCount = m_extraction.GetMeshCount();
		if (!tasks.Extraction.IsValid() || meshCount != tasks.MeshCount)
		{
			tasks.Extraction = BuildExtractionTasks(meshCount);
			tasks.MeshCount = meshCount;
		}
		return ExecuteSystemTasks(tasks.Extraction, executor, *this);
	}

	bool GameWorldState::ExecuteSystems(const GameWorldSystemExecutionContext& context)
	{
		if (!PrepareSystemExecution(context.Resources))
		{
			return false;
		}

		EntityIterationScope iteration(m_registry);
		if (!iteration.IsValid())
		{
			return false;
		}
		GameWorldSystemExecution systems(*this, context);
		const bool bExecuted = systems.Execute(context.Executor);
		if (bExecuted)
		{
			m_dirtyTransforms.clear();
			m_bEvaluateAllTransforms = false;
		}
		return bExecuted;
	}
}
