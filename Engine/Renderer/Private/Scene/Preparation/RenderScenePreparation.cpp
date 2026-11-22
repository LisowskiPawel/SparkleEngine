#include "PCH.h"

#include "Scene/Preparation/RenderScenePreparation.h"

#include "Scene/RenderScene.h"
#include "Scene/Preparation/Deformation/RenderDeformationPreparation.h"
#include "Scene/Preparation/RenderScenePreparationInputResolver.h"
#include "Scene/Preparation/RenderScenePreparationMerger.h"
#include "Scene/Preparation/RenderScenePreparationRun.h"
#include "Scene/Preparation/RenderScenePreparationRanges.h"
#include "Scene/Preparation/PreparedRenderScene.h"

#include <utility>

struct RenderScenePreparation::Impl final
{
	Impl(enkiTaskScheduler* scheduler, GpuMeshCache& gpuMeshCache, TextureCache& textureCache) noexcept;

	void Execute(RenderScene& scene, PreparedRenderScene& output);

private:
	void BeginRun(PreparedRenderScene& output);
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
		PrepareRenderSceneRanges(m_scheduler, m_run);
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
