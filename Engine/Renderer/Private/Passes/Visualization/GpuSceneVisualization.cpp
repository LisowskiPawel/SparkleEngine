#include "../../PCH.h"
#include "Passes/Visualization/GpuSceneVisualization.h"

#include "Core/Public/Math/MathUtils.h"
#include "Frame/Graph/RenderFrameGraphResources.h"
#include "FrameGraph/Builder/FrameGraphBuilder.h"
#include "Passes/Visualization/GpuSceneVisualizationShader.h"

void AddGpuSceneVisualizationPass(FrameGraphBuilder& builder, RenderViewportExtent sceneExtent, const RenderFrameGraphResources& resources)
{
	auto& parameters = builder.AllocParameters<GpuSceneVisualizationCS>();
	parameters->SceneColor = builder.CreateUAV(resources.Transient.Scene.SceneColor);
	parameters->GBufferInstanceIdentity = builder.CreateSRV(resources.Transient.GBuffer.InstanceIdentity);

	const std::uint32_t groupCountX = MathUtils::DivideRoundUp(sceneExtent.Width, 8u);
	const std::uint32_t groupCountY = MathUtils::DivideRoundUp(sceneExtent.Height, 8u);

	builder.Dispatch<GpuSceneVisualizationCS>(parameters, ComputeDispatchDesc{groupCountX, groupCountY, 1u});
}

bool IsGpuSceneVisualizationActive(RenderViewMode viewMode) noexcept
{
	return viewMode == RenderViewMode::GpuSceneInstances;
}
