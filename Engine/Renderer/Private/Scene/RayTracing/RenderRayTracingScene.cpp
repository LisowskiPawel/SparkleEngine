#include "PCH.h"

#include "Scene/RayTracing/RenderRayTracingScene.h"

#include "RHI/Public/Commands/RenderCommandList.h"
#include "Meshes/GpuMeshCache.h"
#include "RayTracing/Acceleration/RayTracingBlasCache.h"
#include "RayTracing/Acceleration/RayTracingAccelerationStrategy.h"
#include "RayTracing/Acceleration/RayTracingPtlasPartitionPlanner.h"
#include "RayTracing/RayTracingCapabilityReport.h"
#include "Scene/Preparation/PreparedRenderScene.h"

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_renderRayTracingSceneLogger, "Renderer.RenderRayTracingScene");

static RayTracingExecutionFrontend SelectExecutionFrontend(const RayTracingCapabilityReport& capabilities) noexcept
{
	const bool bSharedRequirements = capabilities.bSupportsAccelerationStructure && capabilities.bSupportsDescriptorIndexing;
	if (bSharedRequirements && capabilities.bSupportsRayTracingPipeline)
	{
		return RayTracingExecutionFrontend::Pipeline;
	}
	if (bSharedRequirements && capabilities.bSupportsInlineRayQuery)
	{
		return RayTracingExecutionFrontend::Inline;
	}
	return RayTracingExecutionFrontend::None;
}

RenderRayTracingScene::RenderRayTracingScene(RenderHardwareInterface& renderHardwareInterface, const GpuMeshCache& meshes, const RayTracingCapabilityReport& capabilityReport) noexcept :
    m_executionFrontend(SelectExecutionFrontend(capabilityReport))
{
	if (!capabilityReport.bSupportsAccelerationStructure)
	{
		return;
	}

	m_blasCache = std::make_unique<RayTracingBlasCache>(renderHardwareInterface, meshes);
	m_accelerationStrategy = CreateRayTracingAccelerationStrategy(renderHardwareInterface, capabilityReport);
}

RenderRayTracingScene::~RenderRayTracingScene() noexcept = default;

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
    RenderCommandList& commandList,
    const PreparedRenderScene& preparedScene,
    const RayTracingPtlasPartitionPlan& viewPlan,
    PassExecutionDiagnostics* diagnostics) noexcept
{
	if (m_blasCache == nullptr || m_accelerationStrategy == nullptr)
	{
		Diagnostics::Fatal(g_renderRayTracingSceneLogger, __FILE__, __LINE__, "Ray-tracing scene build has no BLAS cache or top-level strategy.");
	}

	m_blasCache->BeginFrame();

	m_accelerationStrategy->Build(commandList, preparedScene, *m_blasCache, m_shaderTablePlan, viewPlan, diagnostics);

	m_blasCache->EndFrame();
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
