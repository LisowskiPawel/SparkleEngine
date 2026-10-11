#include "../../../PCH.h"
#include "Passes/Presentation/PostProcessing/Vignette.h"

#include "Core/Public/Math/MathUtils.h"
#include "Frame/Graph/RenderFrameGraphFormats.h"
#include "Passes/Presentation/PostProcessing/PostProcessingControls.h"
#include "FrameGraph/Builder/FrameGraphBuilder.h"
#include "FrameGraph/FrameGraphTextureDesc.h"
#include "Passes/Presentation/PostProcessing/VignetteShader.h"

ConsoleVariable<bool> CVarVignette("r.Vignette", true, "Enable display-linear edge darkening.");

bool IsVignetteActive(RenderViewMode viewMode) noexcept
{
	return CVarVignette.Get() && IsPostProcessingShown(viewMode);
}

FrameGraphTextureHandle AddVignettePass(FrameGraphBuilder& builder, RenderViewMode viewMode, RenderViewportExtent outputExtent, FrameGraphTextureHandle input)
{
	if (!IsVignetteActive(viewMode))
	{
		return input;
	}

	const FrameGraphTextureHandle output = builder.CreateTexture(FrameGraphTextureDesc::CreateColor("VignetteColor", outputExtent.Width, outputExtent.Height, RenderFrameGraphFormats::SceneColor));
	auto& parameters = builder.AllocParameters<VignetteCS>();
	parameters->DisplayLinearColor = builder.CreateSRV(input);
	parameters->OutputColor = builder.CreateUAV(output);
	builder.Dispatch<VignetteCS>(parameters, ComputeDispatchDesc{MathUtils::DivideRoundUp(outputExtent.Width, 8u), MathUtils::DivideRoundUp(outputExtent.Height, 8u), 1u});
	return output;
}
