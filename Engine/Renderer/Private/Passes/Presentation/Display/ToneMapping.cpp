#include "../../../PCH.h"
#include "Passes/Presentation/Display/ToneMapping.h"

#include "Frame/RenderFrame.h"
#include "Core/Public/Math/MathUtils.h"
#include "Frame/Graph/RenderFrameGraphFormats.h"
#include "Frame/Graph/RenderFrameGraphResources.h"
#include "FrameGraph/Builder/FrameGraphBuilder.h"
#include "FrameGraph/FrameGraphTextureDesc.h"
#include "Passes/Presentation/Display/ToneMappingShader.h"
#include "View/RenderView.h"

FrameGraphTextureHandle AddToneMappingPass(FrameGraphBuilder& builder, const RenderFrame& frame, RenderViewportExtent outputExtent, const RenderFrameGraphResources& resources)
{
	const FrameGraphTextureDesc toneMappedColorDesc = FrameGraphTextureDesc::CreateColor("ToneMappedSceneColor", outputExtent.Width, outputExtent.Height, RenderFrameGraphFormats::SceneColor);
	const FrameGraphTextureHandle toneMappedColor = builder.CreateTexture(toneMappedColorDesc);

	auto& parameters = builder.AllocParameters<ToneMappingCS>();
	parameters->SceneColor = builder.CreateSRV(resources.Presentation.ResolvedSceneColor);
	parameters->ExposureTexture = builder.CreateSRV(resources.Transient.Exposure);
	parameters->ToneMappedColor = builder.CreateUAV(toneMappedColor);

	parameters->ToneMapper = static_cast<std::uint32_t>(frame.View.displaySettings.ToneMapper);

	const std::uint32_t groupCountX = MathUtils::DivideRoundUp(outputExtent.Width, 8u);
	const std::uint32_t groupCountY = MathUtils::DivideRoundUp(outputExtent.Height, 8u);

	builder.Dispatch<ToneMappingCS>(parameters, ComputeDispatchDesc{groupCountX, groupCountY, 1u});

	return toneMappedColor;
}
