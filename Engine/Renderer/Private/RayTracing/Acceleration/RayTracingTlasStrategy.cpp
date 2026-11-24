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

void RayTracingTlasStrategy::Build(
    RenderCommandList& commandList,
    const PreparedRenderScene& preparedScene,
    RayTracingBlasCache& blasCache,
    const RayTracingShaderTablePlan& shaderTablePlan,
    const RayTracingPtlasPartitionPlan& viewPlan,
    PassExecutionDiagnostics* diagnostics) noexcept
{
	(void) viewPlan;
	m_tlasBuilder.Build(commandList, preparedScene, blasCache, shaderTablePlan, diagnostics);
}

void RayTracingTlasStrategy::Clear() noexcept
{
	m_tlasBuilder.Clear();
}
