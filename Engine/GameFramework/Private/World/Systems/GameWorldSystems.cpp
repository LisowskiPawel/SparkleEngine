#include "PCH.h"
#include "World/Systems/GameWorldSystems.h"
#include "World/GameWorldState.h"
#include "World/ECS/EntityIterationScope.h"
#include "World/Resources/GameWorldResourceStores.h"
#include <TaskScheduler_c.h>
#include <algorithm>
#include <atomic>
#include <memory>
#include <new>

namespace ECS
{
	template <class System> struct WorldSystemRange final
	{
		System& Work;
		bool (System::*Kernel)(std::uint32_t, std::uint32_t);
		std::uint32_t Count;
		std::atomic<bool>& bSucceeded;
		std::uint32_t Grain = 1;

		static void Run(std::uint32_t begin, std::uint32_t end, std::uint32_t, void* argument) noexcept
		{
			auto& range = *static_cast<WorldSystemRange*>(argument);
			begin = static_cast<std::uint32_t>((std::min) (std::uint64_t{begin} * range.Grain, std::uint64_t{range.Count}));
			end = static_cast<std::uint32_t>((std::min) (std::uint64_t{end} * range.Grain, std::uint64_t{range.Count}));
			if (begin >= end || !range.bSucceeded.load())
			{
				return;
			}

			try
			{
				if (!(range.Work.*range.Kernel)(begin, end))
				{
					range.bSucceeded = false;
				}
			}
			catch (...)
			{
				range.bSucceeded = false;
			}
		}
	};

	template <class System>
	static auto CreateSystemRange(enkiTaskScheduler* scheduler, WorldSystemRange<System>& range, std::uint32_t grain, std::uint32_t serialThreshold, std::uint32_t maximumPartitions = 16)
	{
		auto destroy = [scheduler](enkiTaskSet* task)
		{
			enkiDeleteTaskSet(scheduler, task);
		};
		auto task = std::unique_ptr<enkiTaskSet, decltype(destroy)>(enkiCreateTaskSet(scheduler, &WorldSystemRange<System>::Run), destroy);
		if (!task)
		{
			throw std::bad_alloc();
		}

		const auto count = (std::max) (range.Count, 1u);
		const auto boundedGrain = (std::max) (grain, 1u + (count - 1) / maximumPartitions);
		const auto minRange = count <= serialThreshold ? count : boundedGrain;
		range.Grain = minRange;
		const auto partitions = 1u + (count - 1) / minRange;
		enkiSetParamsTaskSet(task.get(), {&range, partitions, 1, 0});

		return task;
	}

	static auto DependOnSystem(enkiTaskScheduler* scheduler, enkiTaskSet* prerequisite, enkiTaskSet* dependent)
	{
		auto destroy = [scheduler](enkiDependency* dependency)
		{
			enkiDeleteDependency(scheduler, dependency);
		};
		auto dependency = std::unique_ptr<enkiDependency, decltype(destroy)>(enkiCreateDependency(scheduler), destroy);
		if (!dependency)
		{
			throw std::bad_alloc();
		}

		enkiSetDependency(dependency.get(), enkiGetCompletableFromTaskSet(prerequisite), enkiGetCompletableFromTaskSet(dependent));
		return dependency;
	}

	GameWorldSystemExecution::GameWorldSystemExecution(GameWorldState& state, const GameWorldSystemExecutionContext& context) :
	    m_camera(state.m_registry, state.m_activeCamera, context.Camera),
	    m_animation(state.m_registry, context.Resources.AnimationClips, context.Resources.Skeletons, state.m_animationEvaluation, state.m_morphWeights, context.Camera.DeltaSeconds),
	    m_transform(state.m_registry, state.m_transformWork),
	    m_extraction(state.m_registry, state.m_meshResources, context.Resources.Skeletons, state.m_extraction.GetMeshSlots()),
	    m_state(state)
	{
	}

	bool GameWorldSystemExecution::Execute(enkiTaskScheduler* scheduler)
	{
		if (!m_state.m_registry.IsStructureFrozen())
		{
			return false;
		}
		UpdateCamera();
		return UpdateAnimation(scheduler) && UpdateTransformsAndExtract(scheduler);
	}

	void GameWorldSystemExecution::UpdateCamera()
	{
		const EntityId changed = m_camera.Update();
		if (changed.IsValid())
		{
			m_state.MarkTransformDirty(changed);
			m_state.RecordChange(changed);
		}
	}

	bool GameWorldSystemExecution::UpdateAnimation(enkiTaskScheduler* scheduler)
	{
		if (!scheduler)
		{
			if (!m_animation.RunPlayback(0, m_animation.GetPlaybackCount()))
			{
				return false;
			}
			if (!m_animation.RunPose(0, m_animation.GetPoseCount()))
			{
				return false;
			}
			if (!m_animation.RunMorphSamples(0, m_animation.GetMorphSampleCount()))
			{
				return false;
			}
		}
		else
		{
			if (!ExecuteAnimationRanges(scheduler))
			{
				return false;
			}
		}
		if (!m_animation.CommitMorphOutputs())
		{
			return false;
		}
		return true;
	}

	bool GameWorldSystemExecution::ExecuteAnimationRanges(enkiTaskScheduler* scheduler)
	{
		std::atomic<bool> bSucceeded{true};
		WorldSystemRange playback{m_animation, &AnimationSystemExecution::RunPlayback, m_animation.GetPlaybackCount(), bSucceeded};
		WorldSystemRange pose{m_animation, &AnimationSystemExecution::RunPose, m_animation.GetPoseCount(), bSucceeded};
		WorldSystemRange morph{m_animation, &AnimationSystemExecution::RunMorphSamples, m_animation.GetMorphSampleCount(), bSucceeded};
		auto playbackTask = CreateSystemRange(scheduler, playback, 8, 16);
		auto poseTask = CreateSystemRange(scheduler, pose, 2, 4);
		auto morphTask = CreateSystemRange(scheduler, morph, 8, 16);
		auto poseDependency = DependOnSystem(scheduler, playbackTask.get(), poseTask.get());
		auto morphDependency = DependOnSystem(scheduler, playbackTask.get(), morphTask.get());

		enkiAddTaskSet(scheduler, playbackTask.get());
		enkiWaitForTaskSet(scheduler, poseTask.get());
		enkiWaitForTaskSet(scheduler, morphTask.get());
		return bSucceeded.load();
	}

	bool GameWorldSystemExecution::UpdateTransformsAndExtract(enkiTaskScheduler* scheduler)
	{
		m_state.PrepareTransformWork();
		const auto dirtyCount = m_transform.GetDirtyTransformCount();
		if (!scheduler)
		{
			if (!m_transform.RunTransforms(0, dirtyCount))
			{
				return false;
			}
			if (!m_transform.RunCameraDerivedState(0, dirtyCount))
			{
				return false;
			}
			if (!m_extraction.Run(0, m_extraction.GetMeshCount()))
			{
				return false;
			}
		}
		else
		{
			if (!ExecuteTransformRanges(scheduler, dirtyCount))
			{
				return false;
			}
		}
		m_state.CommitExtraction();
		return true;
	}

	bool GameWorldSystemExecution::ExecuteTransformRanges(enkiTaskScheduler* scheduler, std::uint32_t dirtyCount)
	{
		std::atomic<bool> bSucceeded{true};
		WorldSystemRange transforms{m_transform, &TransformSystemExecution::RunTransforms, dirtyCount, bSucceeded};
		WorldSystemRange cameras{m_transform, &TransformSystemExecution::RunCameraDerivedState, dirtyCount, bSucceeded};
		WorldSystemRange meshes{m_extraction, &MeshExtractionSystemExecution::Run, m_extraction.GetMeshCount(), bSucceeded};
		auto transformTask = CreateSystemRange(scheduler, transforms, 64, 128);
		auto cameraTask = CreateSystemRange(scheduler, cameras, 64, 128);
		auto meshTask = CreateSystemRange(scheduler, meshes, 64, 128);
		auto cameraDependency = DependOnSystem(scheduler, transformTask.get(), cameraTask.get());
		auto meshDependency = DependOnSystem(scheduler, cameraTask.get(), meshTask.get());

		enkiAddTaskSet(scheduler, transformTask.get());
		enkiWaitForTaskSet(scheduler, meshTask.get());
		return bSucceeded.load();
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
		const bool bExecuted = systems.Execute(context.Scheduler);
		if (bExecuted)
		{
			m_dirtyTransforms.clear();
			m_bEvaluateAllTransforms = false;
		}
		return bExecuted;
	}
}
