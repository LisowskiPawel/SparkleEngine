#include "PCH.h"

#include "Scene/Preparation/RenderScenePreparation.h"

#include "Scene/RenderScene.h"
#include "Scene/Preparation/RenderDeformationPreparation.h"
#include "Scene/Preparation/RenderScenePreparationInputResolver.h"
#include "Scene/Preparation/RenderScenePreparationMerger.h"
#include "Scene/Preparation/RenderScenePreparationRun.h"
#include "Scene/Preparation/PreparedRenderScene.h"
#include "Scene/Preparation/RenderScenePreparationGraph.h"
#include "Tasks/Public/TaskExecution.h"
#include "Tasks/Public/TaskExecutionContext.h"
#include "Tasks/Public/TaskExecutor.h"
#include "Tasks/Public/TaskGraph.h"

#include <utility>

struct RenderScenePreparation::Impl final
{
	Impl(TaskExecutor& taskExecutor, GpuMeshCache& gpuMeshCache, TextureCache& textureCache) noexcept;

	void Execute(RenderScene& scene, PreparedRenderScene& output);

private:
	void BeginRun(PreparedRenderScene& output);
	bool ExecuteCompiledGraph();
	void FinishRun(PreparedRenderScene& output) noexcept;
	void EnsureGraph();
	static void ReleaseInputViews(RenderScenePreparationRun& run) noexcept;

	TaskExecutor* m_taskExecutor = nullptr;
	RenderScenePreparationInputResolver m_inputResolver;
	RenderDeformationPreparation m_deformationPreparation;
	RenderScenePreparationRun m_run;
	CompiledTaskGraph m_graph;
	RenderScenePreparationCapacity m_capacity;
};

RenderScenePreparation::Impl::Impl(TaskExecutor& taskExecutor, GpuMeshCache& gpuMeshCache, TextureCache& textureCache) noexcept :
    m_taskExecutor(&taskExecutor),
    m_inputResolver(gpuMeshCache, textureCache)
{
}

void RenderScenePreparation::Impl::Execute(RenderScene& scene, PreparedRenderScene& output)
{
	BeginRun(output);
	m_inputResolver.Resolve(scene, m_deformationPreparation, m_run);
	EnsureGraph();

	if (!ExecuteCompiledGraph())
	{
		scene.ResetContinuity();
		m_run.PreparedScene.ResetForReuse();
		FinishRun(output);
		return;
	}

	scene.CommitContinuity(m_run.PreparedPrimitives, m_run.Deformation);
	RenderScenePreparationMerger::PublishSceneOutputs(m_run);
	FinishRun(output);
}

void RenderScenePreparation::Impl::BeginRun(PreparedRenderScene& output)
{
	m_run.PreparedScene = std::move(output);
	m_run.Deformation.JointMatrices = std::move(m_run.PreparedScene.jointMatrices);
	m_run.Deformation.PreviousJointMatrices = std::move(m_run.PreparedScene.previousJointMatrices);
	m_run.Deformation.MorphWeights = std::move(m_run.PreparedScene.morphWeights);
	m_run.Deformation.PreviousMorphWeights = std::move(m_run.PreparedScene.previousMorphWeights);
	m_run.PreparedScene.ResetForReuse();
}

bool RenderScenePreparation::Impl::ExecuteCompiledGraph()
{
	if (!m_graph)
	{
		return false;
	}

	TaskExecutionContext context(m_run);
	const TaskExecution execution = m_taskExecutor->Submit(m_graph, context);
	return execution.GetStatus() == TaskExecutionStatus::Succeeded;
}

void RenderScenePreparation::Impl::FinishRun(PreparedRenderScene& output) noexcept
{
	ReleaseInputViews(m_run);
	output = std::move(m_run.PreparedScene);
}

void RenderScenePreparation::Impl::EnsureGraph()
{
	const RenderScenePreparationCapacity capacity = ResolveRenderScenePreparationCapacity(m_run);
	if (m_graph && capacity == m_capacity)
	{
		return;
	}

	m_graph = BuildRenderScenePreparationGraph(capacity);
	m_capacity = capacity;
}

void RenderScenePreparation::Impl::ReleaseInputViews(RenderScenePreparationRun& run) noexcept
{
	run.Lights = {};
	run.Deformation.JointMatrixCopyRanges.clear();
	run.Deformation.MorphWeightCopyRanges.clear();
}

RenderScenePreparation::RenderScenePreparation(TaskExecutor& taskExecutor, GpuMeshCache& gpuMeshCache, TextureCache& textureCache) :
    m_impl(std::make_unique<Impl>(taskExecutor, gpuMeshCache, textureCache))
{
}

RenderScenePreparation::~RenderScenePreparation() noexcept = default;

void RenderScenePreparation::Execute(RenderScene& scene, PreparedRenderScene& output)
{
	m_impl->Execute(scene, output);
}
