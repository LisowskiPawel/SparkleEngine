#include "PCH.h"
#include "Scene/Preparation/RenderScenePreparationRanges.h"

#include "Scene/Preparation/RenderScenePreparationRun.h"
#include "Scene/Preparation/Deformation/RenderDeformationPreparation.h"
#include "Scene/Preparation/Geometry/RenderPrimitivePreparation.h"
#include "Scene/Preparation/Lighting/RenderLightPreparation.h"
#include <TaskScheduler_c.h>
#include <algorithm>
#include <array>
#include <limits>
#include <memory>
#include <new>
#include <stdexcept>

enum class ScenePreparationBranch : std::uint8_t
{
	Primitives,
	Joints,
	MorphWeights,
	Lights
};

struct ScenePreparationRange final
{
	RenderScenePreparationRun& Run;
	ScenePreparationBranch Branch;
	std::uint32_t Count = 0;
	std::uint32_t Grain = 1;

	static void Execute(std::uint32_t begin, std::uint32_t end, std::uint32_t, void* argument) noexcept
	{
		auto& range = *static_cast<ScenePreparationRange*>(argument);
		begin = static_cast<std::uint32_t>((std::min) (std::uint64_t{begin} * range.Grain, std::uint64_t{range.Count}));
		end = static_cast<std::uint32_t>((std::min) (std::uint64_t{end} * range.Grain, std::uint64_t{range.Count}));
		auto& run = range.Run;
		switch (range.Branch)
		{
			case ScenePreparationBranch::Primitives:
			{
				const auto inputs = std::span<const ResolvedRenderPrimitive>(run.ResolvedPrimitives).subspan(begin, end - begin);
				const auto outputs = std::span<PreparedRenderPrimitive>(run.PreparedPrimitives).subspan(begin, end - begin);
				PrepareRenderPrimitives(inputs, outputs);
				break;
			}
			case ScenePreparationBranch::Joints:
			{
				const auto ranges = std::span<const RenderJointMatrixCopyRange>(run.Deformation.JointMatrixCopyRanges).subspan(begin, end - begin);
				CopyRenderJointMatrixRanges(ranges, run.Deformation.JointMatrices, run.Deformation.PreviousJointMatrices);
				break;
			}
			case ScenePreparationBranch::MorphWeights:
			{
				const auto ranges = std::span<const RenderMorphWeightCopyRange>(run.Deformation.MorphWeightCopyRanges).subspan(begin, end - begin);
				CopyRenderMorphWeightRanges(ranges, run.Deformation.MorphWeights, run.Deformation.PreviousMorphWeights);
				break;
			}
			case ScenePreparationBranch::Lights:
			{
				const auto inputs = run.Lights.subspan(begin, end - begin);
				const auto outputs = std::span<PreparedRenderLight>(run.PreparedLights).subspan(begin, end - begin);
				PrepareRenderLights(inputs, outputs);
				break;
			}
		}
	}
};

void PrepareRenderSceneRanges(enkiTaskScheduler* scheduler, RenderScenePreparationRun& run)
{
	std::array ranges{
	    ScenePreparationRange{run, ScenePreparationBranch::Primitives},
	    ScenePreparationRange{run, ScenePreparationBranch::Joints},
	    ScenePreparationRange{run, ScenePreparationBranch::MorphWeights},
	    ScenePreparationRange{run, ScenePreparationBranch::Lights}};
	const std::array counts{run.ResolvedPrimitives.size(), run.Deformation.JointMatrixCopyRanges.size(), run.Deformation.MorphWeightCopyRanges.size(), run.PreparedLights.size()};
	const std::array thresholds{128u, 64u, 64u, 32u};
	const std::array grains{64u, 16u, 16u, 16u};
	const std::array maximumPartitions{8u, 8u, 8u, 4u};
	auto destroy = [scheduler = scheduler](enkiTaskSet* task)
	{
		enkiDeleteTaskSet(scheduler, task);
	};
	using NativeTask = std::unique_ptr<enkiTaskSet, decltype(destroy)>;
	std::array<NativeTask, 4> tasks{NativeTask{nullptr, destroy}, NativeTask{nullptr, destroy}, NativeTask{nullptr, destroy}, NativeTask{nullptr, destroy}};
	for (std::size_t branch = 0; branch < ranges.size(); ++branch)
	{
		if (counts[branch] > (std::numeric_limits<std::uint32_t>::max)())
		{
			throw std::length_error("Scene preparation range exceeds native indexing.");
		}
		const auto count = static_cast<std::uint32_t>(counts[branch]);
		ranges[branch].Count = count;
		if (!scheduler || count <= thresholds[branch])
		{
			ScenePreparationRange::Execute(0, count, 0, &ranges[branch]);
			continue;
		}
		tasks[branch].reset(enkiCreateTaskSet(scheduler, &ScenePreparationRange::Execute));
		if (!tasks[branch])
		{
			throw std::bad_alloc();
		}
		const auto grain = (std::max) (grains[branch], 1u + (count - 1) / maximumPartitions[branch]);
		ranges[branch].Grain = grain;
		const auto partitions = 1u + (count - 1) / grain;
		enkiSetParamsTaskSet(tasks[branch].get(), {&ranges[branch], partitions, 1, 0});
	}
	// No admission until all fallible native preparation is complete. Each range
	// is a leaf: native completion follows its last body; no post callback borrows.
	for (auto& task : tasks)
	{
		if (task)
		{
			enkiAddTaskSet(scheduler, task.get());
		}
	}
	for (auto& task : tasks)
	{
		if (task)
		{
			enkiWaitForTaskSet(scheduler, task.get());
		}
	}
}
