#include "../../PCH.h"
#include "Passes/Visualization/GBufferVisualization.h"

#include "Frame/RenderFrame.h"
#include "Core/Public/Math/MathUtils.h"
#include "Frame/Graph/RenderFrameGraphResources.h"
#include "FrameGraph/Builder/FrameGraphBuilder.h"
#include "Passes/Visualization/GBufferVisualizationShader.h"
#include "View/RenderView.h"

void AddGBufferVisualizationPass(FrameGraphBuilder& builder, const RenderFrame& frame, RenderViewportExtent sceneExtent, const RenderFrameGraphResources& resources)
{
	const GBufferRenderTargets& gbuffer = resources.Transient.GBuffer;

	auto& parameters = builder.AllocParameters<GBufferVisualizationCS>();
	parameters->SceneColor = builder.CreateUAV(resources.Transient.Scene.SceneColor);
	parameters->GBufferBaseColor = builder.CreateSRV(gbuffer.BaseColor);
	parameters->GBufferWorldNormal = builder.CreateSRV(gbuffer.WorldNormal);
	parameters->GBufferWorldTangent = builder.CreateSRV(gbuffer.WorldTangent);
	parameters->GBufferMaterial = builder.CreateSRV(gbuffer.Material);
	parameters->GBufferEmissive = builder.CreateSRV(gbuffer.Emissive);
	parameters->GBufferSubsurface = builder.CreateSRV(gbuffer.Subsurface);

	parameters->View = frame.View.uniform;

	const std::uint32_t groupCountX = MathUtils::DivideRoundUp(sceneExtent.Width, 8u);
	const std::uint32_t groupCountY = MathUtils::DivideRoundUp(sceneExtent.Height, 8u);

	builder.Dispatch<GBufferVisualizationCS>(parameters, ComputeDispatchDesc{groupCountX, groupCountY, 1u});
}
