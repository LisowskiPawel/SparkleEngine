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

	const FrameGraphTextureDesc outputDesc = FrameGraphTextureDesc::CreateColor("VignetteColor", outputExtent.Width, outputExtent.Height, RenderFrameGraphFormats::SceneColor);
	const FrameGraphTextureHandle output = builder.CreateTexture(outputDesc);
	auto& parameters = builder.AllocParameters<VignetteCS>();
	parameters->DisplayLinearColor = builder.CreateSRV(input);
	parameters->OutputColor = builder.CreateUAV(output);

	const std::uint32_t groupCountX = MathUtils::DivideRoundUp(outputExtent.Width, 8u);
	const std::uint32_t groupCountY = MathUtils::DivideRoundUp(outputExtent.Height, 8u);

	builder.Dispatch<VignetteCS>(parameters, ComputeDispatchDesc{groupCountX, groupCountY, 1u});
	return output;
}
