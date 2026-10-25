#include "PCH.h"

#include "RayTracing/Acceleration/RayTracingClassicTlasStrategy.h"

#include "Scene/Preparation/PreparedRenderScene.h"

RayTracingClassicTlasStrategy::RayTracingClassicTlasStrategy(RenderHardwareInterface& renderHardwareInterface) noexcept :
    m_classicTlasBuilder(renderHardwareInterface)
{
}

RayTracingClassicTlasStrategy::~RayTracingClassicTlasStrategy() noexcept = default;

RenderRayTracingFrameBindings RayTracingClassicTlasStrategy::Prepare(const PreparedRenderScene& preparedScene, const RayTracingPtlasPartitionPlan& viewPlan) noexcept
{
	(void) viewPlan;
	RenderRayTracingFrameBindings frameBindings{};
	const std::uint32_t estimatedInstanceCount = static_cast<std::uint32_t>(preparedScene.rayTracingInstances.size());
	m_classicTlasBuilder.Prepare(estimatedInstanceCount);

	frameBindings.TlasResource = m_classicTlasBuilder.GetResource();
	frameBindings.EstimatedInstanceCount = estimatedInstanceCount;
	return frameBindings;
}

RayTracingTopLevelAccelerationStructureBuildResult RayTracingClassicTlasStrategy::Build(
    RenderCommandContext& commandContext,
    const PreparedRenderScene& preparedScene,
    RayTracingBlasCache& blasCache,
    const RayTracingShaderTablePlan& shaderTablePlan,
    const RayTracingPtlasPartitionPlan& viewPlan,
    RayTracingPerformanceDiagnostics* diagnostics) noexcept
{
	(void) viewPlan;
	RayTracingTopLevelAccelerationStructureBuildResult result{};
	result.ActiveProvider = ERhiRayTracingTopLevelProvider::ClassicTlas;
	result.ActiveProviderReason = "classic-tlas-strategy-selected";
	result.InstanceCount = m_classicTlasBuilder.Build(commandContext, preparedScene, blasCache, shaderTablePlan, diagnostics);
	return result;
}

void RayTracingClassicTlasStrategy::Clear() noexcept
{
	m_classicTlasBuilder.Clear();
}
