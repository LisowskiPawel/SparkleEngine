#include "PCH.h"

#include "View/RenderViewPreparation.h"

#include "Debug/RendererCVars.h"
#include "Scene/Preparation/PreparedRenderScene.h"
#include "Tasks/Public/TaskExecution.h"
#include "Tasks/Public/TaskExecutionContext.h"
#include "Tasks/Public/TaskExecutor.h"
#include "View/RenderView.h"
#include "View/RenderViewState.h"

#include <utility>

RenderViewPreparation::RenderViewPreparation(TaskExecutor& taskExecutor) noexcept :
    m_taskExecutor(&taskExecutor)
{
}

void RenderViewPreparation::Prepare(const PreparedRenderScene& preparedScene, RenderView& view, RenderViewState& viewState)
{
	BeginPreparation(preparedScene, view);
	if (!ExecuteVisibility())
	{
		view.rasterPrimitiveIndices.clear();
		view.meshInstanceBatches.clear();
		view.workload = {};
		ReleaseInputViews();
		return;
	}

	BuildMeshBatches(preparedScene, view);

	BuildRenderViewWorkload(preparedScene, view);
	view.rayTracingPlan = viewState.BuildRayTracingPlan(preparedScene, view.cameraUniform.Position);

	ReleaseInputViews();
}

void RenderViewPreparation::BeginPreparation(const PreparedRenderScene& preparedScene, RenderView& view)
{
	m_run.Scene = &preparedScene;
	m_run.View = &view;
	m_run.Items.resize(preparedScene.primitives.size());
	EnsureGraph(preparedScene.primitives.size());
}

bool RenderViewPreparation::ExecuteVisibility()
{
	TaskExecutionContext context(m_run);
	const TaskExecution execution = m_taskExecutor->Submit(m_graph, context);
	return execution.GetStatus() == TaskExecutionStatus::Succeeded;
}

void RenderViewPreparation::BuildMeshBatches(const PreparedRenderScene& preparedScene, RenderView& view)
{
	m_batchResult.RasterInstanceIndices = std::move(view.rasterPrimitiveIndices);
	m_batchResult.Batches = std::move(view.meshInstanceBatches);

	m_batchBuilder.Build(
	    m_run.Items,
	    preparedScene.primitives,
	    preparedScene.instanceGroups,
	    MeshInstanceBatchBuildOptions{.EnableAutoBatching = CVarRendererMeshAutoBatching.Get(), .RequireMaterialBindingSet = true, .CollectDiagnostics = false},
	    m_batchResult);

	view.rasterPrimitiveIndices = std::move(m_batchResult.RasterInstanceIndices);
	view.meshInstanceBatches = std::move(m_batchResult.Batches);
}

void RenderViewPreparation::ReleaseInputViews() noexcept
{
	m_run.Scene = nullptr;
	m_run.View = nullptr;
}
