#include "PCH.h"

#include "RayTracing/Acceleration/RayTracingTlasStrategy.h"

#include "Scene/Preparation/PreparedRenderScene.h"

RayTracingTlasStrategy::RayTracingTlasStrategy(RenderHardwareInterface& renderHardwareInterface) noexcept :
    m_tlasBuilder(renderHardwareInterface)
{
}

RayTracingTlasStrategy::~RayTracingTlasStrategy() noexcept = default;

RenderRayTracingFrameBindings RayTracingTlasStrategy::Prepare(const PreparedRenderScene& preparedScene, const RayTracingPtlasPartitionPlan& viewPlan) noexcept
{
	(void) viewPlan;
	RenderRayTracingFrameBindings frameBindings{};
	const std::uint32_t estimatedInstanceCount = static_cast<std::uint32_t>(preparedScene.rayTracingInstances.size());
	m_tlasBuilder.Prepare(estimatedInstanceCount);

	frameBindings.TlasResource = m_tlasBuilder.GetResource();
	frameBindings.EstimatedInstanceCount = estimatedInstanceCount;
	return frameBindings;
}

RayTracingTlasBuildResult RayTracingTlasStrategy::Build(
    RenderCommandContext& commandContext,
    const PreparedRenderScene& preparedScene,
    RayTracingBlasCache& blasCache,
    const RayTracingShaderTablePlan& shaderTablePlan,
    const RayTracingPtlasPartitionPlan& viewPlan,
    RayTracingPerformanceDiagnostics* diagnostics) noexcept
{
	(void) viewPlan;
	RayTracingTlasBuildResult result{};
	result.ActiveProvider = ERhiRayTracingTlasProvider::Tlas;
	result.ActiveProviderReason = "TLAS-strategy-selected";
	result.InstanceCount = m_tlasBuilder.Build(commandContext, preparedScene, blasCache, shaderTablePlan, diagnostics);
	return result;
}

void RayTracingTlasStrategy::Clear() noexcept
{
	m_tlasBuilder.Clear();
}
