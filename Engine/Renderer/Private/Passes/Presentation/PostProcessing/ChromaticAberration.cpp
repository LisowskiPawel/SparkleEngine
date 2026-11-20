#include "../../../PCH.h"
#include "Passes/Presentation/PostProcessing/ChromaticAberration.h"

#include "Core/Public/Math/MathUtils.h"
#include "Frame/Graph/RenderFrameGraphFormats.h"
#include "Passes/Presentation/PostProcessing/PostProcessingControls.h"
#include "FrameGraph/Builder/FrameGraphBuilder.h"
#include "FrameGraph/FrameGraphTextureDesc.h"
#include "Passes/Presentation/PostProcessing/ChromaticAberrationShader.h"

ConsoleVariable<bool> CVarChromaticAberration("r.ChromaticAberration", true, "Enable display-linear radial RGB separation.");

bool IsChromaticAberrationActive(RenderViewMode viewMode) noexcept
{
	return CVarChromaticAberration.Get() && IsPostProcessingShown(viewMode);
}

FrameGraphTextureHandle AddChromaticAberrationPass(FrameGraphBuilder& builder, RenderViewMode viewMode, RenderViewportExtent outputExtent, FrameGraphTextureHandle input)
{
	if (!IsChromaticAberrationActive(viewMode))
	{
		return input;
	}

	const FrameGraphTextureDesc outputDesc = FrameGraphTextureDesc::CreateColor("ChromaticAberrationColor", outputExtent.Width, outputExtent.Height, RenderFrameGraphFormats::SceneColor);
	const FrameGraphTextureHandle output = builder.CreateTexture(outputDesc);

	auto& parameters = builder.AllocParameters<ChromaticAberrationCS>();
	parameters->DisplayLinearColor = builder.CreateSRV(input);
	parameters->OutputColor = builder.CreateUAV(output);

	const std::uint32_t groupCountX = MathUtils::DivideRoundUp(outputExtent.Width, 8u);
	const std::uint32_t groupCountY = MathUtils::DivideRoundUp(outputExtent.Height, 8u);

	builder.Dispatch<ChromaticAberrationCS>(parameters, ComputeDispatchDesc{groupCountX, groupCountY, 1u});
	return output;
}
