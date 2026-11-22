#include "PCH.h"

#include "Scene/Preparation/RenderScenePreparation.h"

#include "Scene/RenderScene.h"
#include "Scene/Preparation/Deformation/RenderDeformationPreparation.h"
#include "Scene/Preparation/RenderScenePreparationInputResolver.h"
#include "Scene/Preparation/RenderScenePreparationMerger.h"
#include "Scene/Preparation/RenderScenePreparationRun.h"
#include "Scene/Preparation/PreparedRenderScene.h"
#include "Scene/Preparation/Geometry/RenderPrimitivePreparation.h"
#include "Scene/Preparation/Lighting/RenderLightPreparation.h"
#include <TaskScheduler_c.h>
#include <algorithm>
#include <array>
#include <limits>
#include <new>
#include <stdexcept>

#include <utility>

struct RenderScenePreparation::Impl final
{
	Impl(enkiTaskScheduler* scheduler, GpuMeshCache& gpuMeshCache, TextureCache& textureCache) noexcept;

	void Execute(RenderScene& scene, PreparedRenderScene& output);

private:
	void BeginRun(PreparedRenderScene& output);
	void ExecuteRanges();
	void FinishRun(PreparedRenderScene& output) noexcept;
	static void ReleaseInputViews(RenderScenePreparationRun& run) noexcept;

	enkiTaskScheduler* m_scheduler = nullptr;
	const GpuMeshCache* m_gpuMeshCache = nullptr;
	const TextureCache* m_textureCache = nullptr;
	RenderScenePreparationRun m_run;
};

RenderScenePreparation::Impl::Impl(enkiTaskScheduler* scheduler, GpuMeshCache& gpuMeshCache, TextureCache& textureCache) noexcept :
    m_scheduler(scheduler),
    m_gpuMeshCache(&gpuMeshCache),
    m_textureCache(&textureCache)
{
}

void RenderScenePreparation::Impl::Execute(RenderScene& scene, PreparedRenderScene& output)
{
	BeginRun(output);
	try
	{
		ResolveRenderScenePreparationInputs(scene, *m_gpuMeshCache, *m_textureCache, m_run);
		PrepareRenderDeformation(scene, m_run.ResolvedPrimitives, m_run.Deformation);
		ExecuteRanges();
		RenderScenePreparationMerger::Merge(m_run);
		RenderScenePreparationMerger::BuildRayTracingPlan(m_run);
		scene.CommitContinuity(m_run.PreparedScene.primitives, m_run.Deformation);
		RenderScenePreparationMerger::PublishSceneOutputs(m_run);
	}
	catch (...)
	{
		scene.ResetContinuity();
		m_run.PreparedScene.ResetForReuse();
		FinishRun(output);
		throw;
	}

	FinishRun(output);
}

void RenderScenePreparation::Impl::BeginRun(PreparedRenderScene& output)
{
	m_run.PreparedScene = std::move(output);
	m_run.PreparedPrimitives = std::move(m_run.PreparedScene.primitives);
	m_run.Deformation.JointMatrices = std::move(m_run.PreparedScene.jointMatrices);
	m_run.Deformation.PreviousJointMatrices = std::move(m_run.PreparedScene.previousJointMatrices);
	m_run.Deformation.MorphWeights = std::move(m_run.PreparedScene.morphWeights);
	m_run.Deformation.PreviousMorphWeights = std::move(m_run.PreparedScene.previousMorphWeights);
	m_run.PreparedScene.ResetForReuse();
}

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

void RenderScenePreparation::Impl::ExecuteRanges()
{
	std::array ranges{
	    ScenePreparationRange{m_run, ScenePreparationBranch::Primitives},
	    ScenePreparationRange{m_run, ScenePreparationBranch::Joints},
	    ScenePreparationRange{m_run, ScenePreparationBranch::MorphWeights},
	    ScenePreparationRange{m_run, ScenePreparationBranch::Lights}};
	const std::array counts{m_run.ResolvedPrimitives.size(), m_run.Deformation.JointMatrixCopyRanges.size(), m_run.Deformation.MorphWeightCopyRanges.size(), m_run.PreparedLights.size()};
	const std::array thresholds{128u, 64u, 64u, 32u};
	const std::array grains{64u, 16u, 16u, 16u};
	const std::array maximumPartitions{8u, 8u, 8u, 4u};
	auto destroy = [scheduler = m_scheduler](enkiTaskSet* task)
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
		if (!m_scheduler || count <= thresholds[branch])
		{
			ScenePreparationRange::Execute(0, count, 0, &ranges[branch]);
			continue;
		}
		tasks[branch].reset(enkiCreateTaskSet(m_scheduler, &ScenePreparationRange::Execute));
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
			enkiAddTaskSet(m_scheduler, task.get());
		}
	}
	for (auto& task : tasks)
	{
		if (task)
		{
			enkiWaitForTaskSet(m_scheduler, task.get());
		}
	}
}

void RenderScenePreparation::Impl::FinishRun(PreparedRenderScene& output) noexcept
{
	ReleaseInputViews(m_run);
	output = std::move(m_run.PreparedScene);
}

void RenderScenePreparation::Impl::ReleaseInputViews(RenderScenePreparationRun& run) noexcept
{
	run.Lights = {};
	run.Deformation.JointMatrixCopyRanges.clear();
	run.Deformation.MorphWeightCopyRanges.clear();
}

RenderScenePreparation::RenderScenePreparation(enkiTaskScheduler* scheduler, GpuMeshCache& gpuMeshCache, TextureCache& textureCache) :
    m_impl(std::make_unique<Impl>(scheduler, gpuMeshCache, textureCache))
{
}

RenderScenePreparation::~RenderScenePreparation() noexcept = default;

void RenderScenePreparation::Execute(RenderScene& scene, PreparedRenderScene& output)
{
	m_impl->Execute(scene, output);
}
