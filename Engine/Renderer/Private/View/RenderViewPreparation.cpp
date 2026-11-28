#include "PCH.h"

#include "View/RenderViewPreparation.h"

#include "Debug/RendererCVars.h"
#include "Scene/Preparation/PreparedRenderScene.h"
#include "View/RenderView.h"
#include "View/RenderViewState.h"

#include <TaskScheduler_c.h>
#include <algorithm>
#include <limits>
#include <memory>
#include <new>
#include <stdexcept>
#include <utility>

struct VisibilityRange final
{
	RenderViewVisibilityRun& Run;
	std::uint32_t Grain;

	static void Execute(std::uint32_t begin, std::uint32_t end, std::uint32_t, void* argument) noexcept
	{
		auto& range = *static_cast<VisibilityRange*>(argument);
		const auto count = range.Run.Items.size();
		begin = static_cast<std::uint32_t>((std::min) (std::uint64_t{begin} * range.Grain, std::uint64_t{count}));
		end = static_cast<std::uint32_t>((std::min) (std::uint64_t{end} * range.Grain, std::uint64_t{count}));
		EvaluateRenderViewVisibility(range.Run, begin, end);
	}
};

RenderViewPreparation::RenderViewPreparation(enkiTaskScheduler* scheduler) noexcept :
    m_scheduler(scheduler)
{
}

void RenderViewPreparation::Prepare(const PreparedRenderScene& preparedScene, RenderView& view, RenderViewState& viewState)
{
	try
	{
		BeginPreparation(preparedScene, view);
		ExecuteVisibility();
		BuildMeshBatches(preparedScene, view);
		BuildRenderViewWorkload(preparedScene, view);
		view.rayTracingPlan = viewState.BuildRayTracingPlan(preparedScene, view.cameraUniform.Position);
	}
	catch (...)
	{
		ReleaseInputViews();
		throw;
	}
	ReleaseInputViews();
}

void RenderViewPreparation::BeginPreparation(const PreparedRenderScene& preparedScene, RenderView& view)
{
	m_run.Scene = &preparedScene;
	m_run.View = &view;
	m_run.Items.resize(preparedScene.primitives.size());
}

void RenderViewPreparation::ExecuteVisibility()
{
	const auto size = m_run.Items.size();
	if (size > (std::numeric_limits<std::uint32_t>::max)())
	{
		throw std::length_error("Visibility range exceeds native indexing.");
	}
	const auto count = static_cast<std::uint32_t>(size);
	if (!m_scheduler || count <= 128)
	{
		EvaluateRenderViewVisibility(m_run, 0, count);
		return;
	}
	auto destroy = [scheduler = m_scheduler](enkiTaskSet* task)
	{
		enkiDeleteTaskSet(scheduler, task);
	};
	auto task = std::unique_ptr<enkiTaskSet, decltype(destroy)>(enkiCreateTaskSet(m_scheduler, &VisibilityRange::Execute), destroy);
	if (!task)
	{
		throw std::bad_alloc();
	}
	const auto grain = (std::max) (64u, 1u + (count - 1) / 8u);
	const auto partitions = 1u + (count - 1) / grain;
	VisibilityRange range{m_run, grain};
	enkiAddTaskSetMinRange(m_scheduler, task.get(), &range, partitions, 1);
	enkiWaitForTaskSet(m_scheduler, task.get());
}

void RenderViewPreparation::BuildMeshBatches(const PreparedRenderScene& preparedScene, RenderView& view)
{
	m_batchResult.RasterInstanceIndices = std::move(view.rasterPrimitiveIndices);
	m_batchResult.Batches = std::move(view.meshInstanceBatches);

	m_batchBuilder.Build(
	    m_run.Items,
	    preparedScene.primitives,
	    preparedScene.instanceGroups,
	    CVarRendererMeshAutoBatching.Get(),
	    m_batchResult);

	view.rasterPrimitiveIndices = std::move(m_batchResult.RasterInstanceIndices);
	view.meshInstanceBatches = std::move(m_batchResult.Batches);
}

void RenderViewPreparation::ReleaseInputViews() noexcept
{
	m_run.Scene = nullptr;
	m_run.View = nullptr;
}
