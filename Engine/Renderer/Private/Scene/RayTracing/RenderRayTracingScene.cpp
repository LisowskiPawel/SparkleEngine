#include "PCH.h"

#include "Scene/RayTracing/RenderRayTracingScene.h"

#include "Commands/RenderCommandContext.h"
#include "Meshes/GpuMeshCache.h"
#include "RayTracing/Acceleration/RayTracingBlasCache.h"
#include "RayTracing/Diagnostics/RayTracingPerformanceDiagnostics.h"
#include "RayTracing/Acceleration/RayTracingAccelerationStrategy.h"
#include "RayTracing/Acceleration/RayTracingPtlasPartitionPlanner.h"
#include "RayTracing/RayTracingCapabilityReport.h"
#include "Scene/Preparation/PreparedRenderScene.h"

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_renderRayTracingSceneLogger, "Renderer.RenderRayTracingScene");

static RayTracingExecutionFrontend SelectExecutionFrontend(const RayTracingCapabilityReport& capabilities) noexcept
{
	const bool sharedRequirements = capabilities.SupportsAccelerationStructure && capabilities.SupportsDescriptorIndexing;
	if (sharedRequirements && capabilities.SupportsRayTracingPipeline)
	{
		return RayTracingExecutionFrontend::Pipeline;
	}
	if (sharedRequirements && capabilities.SupportsInlineRayQuery)
	{
		return RayTracingExecutionFrontend::Inline;
	}
	return RayTracingExecutionFrontend::None;
}

RenderRayTracingScene::RenderRayTracingScene(RenderHardwareInterface& renderHardwareInterface, const GpuMeshCache& meshes, const RayTracingCapabilityReport& capabilityReport) noexcept :
    m_executionFrontend(SelectExecutionFrontend(capabilityReport))
{
	m_performanceMetrics.Providers.TlasProvider = capabilityReport.TlasProvider.SelectedProvider;
	m_performanceMetrics.Providers.TlasProviderReason = capabilityReport.TlasProvider.SelectionReason;
	m_performanceMetrics.Providers.PtlasProvider = capabilityReport.Ptlas.Provider;
	m_performanceMetrics.Providers.SupportsPtlas = capabilityReport.Ptlas.Supported;
	m_performanceMetrics.Providers.PtlasCapabilityReason = capabilityReport.Ptlas.CapabilityStatusReason;
	if (!capabilityReport.SupportsAccelerationStructure)
	{
		return;
	}

	m_blasCache = std::make_unique<RayTracingBlasCache>(renderHardwareInterface, meshes);
	m_accelerationStrategy = CreateRayTracingAccelerationStrategy(renderHardwareInterface, capabilityReport);
}

RenderRayTracingScene::~RenderRayTracingScene() noexcept = default;

void RenderRayTracingScene::BeginGraphBuild() noexcept
{
	if (m_executionFrontend == RayTracingExecutionFrontend::Pipeline)
	{
		m_shaderTablePlan.BeginMaterializationSet();
	}
}

std::uint64_t RenderRayTracingScene::GetGraphGeneration() const noexcept
{
	return m_executionFrontend == RayTracingExecutionFrontend::Pipeline ? m_shaderTablePlan.GetGeneration() : 0u;
}

RenderRayTracingFrameBindings RenderRayTracingScene::Prepare(const PreparedRenderScene& preparedScene, const RayTracingPtlasPartitionPlan& viewPlan) noexcept
{
	if (m_accelerationStrategy == nullptr)
	{
		Diagnostics::Fatal(g_renderRayTracingSceneLogger, __FILE__, __LINE__, "Ray-tracing scene has no TLAS strategy.");
	}
	return m_accelerationStrategy->Prepare(preparedScene, viewPlan);
}

void RenderRayTracingScene::Build(
    RenderCommandContext& commandContext,
    const PreparedRenderScene& preparedScene,
    const RayTracingPtlasPartitionPlan& viewPlan,
    PassExecutionDiagnostics* diagnostics) noexcept
{
	RayTracingPerformanceDiagnostics performanceDiagnostics{diagnostics};

	if (m_blasCache == nullptr || m_accelerationStrategy == nullptr)
	{
		Diagnostics::Fatal(g_renderRayTracingSceneLogger, __FILE__, __LINE__, "Ray-tracing scene build has no BLAS cache or top-level strategy.");
	}

	m_blasCache->BeginFrame();

	const RayTracingTlasBuildResult topLevelBuild = m_accelerationStrategy->Build(commandContext, preparedScene, *m_blasCache, m_shaderTablePlan, viewPlan, &performanceDiagnostics);

	const RayTracingBlasCache::BuildStats blasStats = m_blasCache->EndFrame();

	m_performanceMetrics.Providers.TlasProvider = topLevelBuild.ActiveProvider;
	m_performanceMetrics.Providers.TlasProviderReason = topLevelBuild.ActiveProviderReason;
	m_performanceMetrics.Blas.ReferencedMeshCount = blasStats.referencedMeshCount;
	m_performanceMetrics.Blas.BuiltCount = blasStats.builtBlasCount;
	m_performanceMetrics.Blas.ReusedCount = blasStats.reusedBlasCount;
	m_performanceMetrics.TopLevelInstanceCount = topLevelBuild.InstanceCount;
}

void RenderRayTracingScene::Clear() noexcept
{
	m_shaderTablePlan.Clear();
	if (m_accelerationStrategy != nullptr)
	{
		m_accelerationStrategy->Clear();
	}

	if (m_blasCache != nullptr)
	{
		m_blasCache->Clear();
	}
}

void RenderRayTracingScene::SynchronizeShaderTablePlan(std::span<const RenderPrimitive> primitives, const RenderMaterialTable& materials) noexcept
{
	m_shaderTablePlan.Synchronize(primitives, materials);
}
