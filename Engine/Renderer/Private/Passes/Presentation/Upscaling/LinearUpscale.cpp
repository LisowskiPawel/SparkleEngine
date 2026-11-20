#include "../../../PCH.h"
#include "Passes/Presentation/Upscaling/LinearUpscale.h"

#include "Core/Public/Math/MathUtils.h"
#include "FrameGraph/Builder/FrameGraphBuilder.h"
#include "Passes/Presentation/Upscaling/LinearUpscaleShader.h"
#include "RHI/Public/Samplers/RhiSamplerDesc.h"

void AddLinearUpscalePass(FrameGraphBuilder& builder, FrameGraphTextureHandle inputColor, FrameGraphTextureHandle outputColor, RenderViewportExtent outputExtent)
{
	auto& parameters = builder.AllocParameters<LinearUpscaleCS>();
	parameters->ScalingInputColor = builder.CreateSRV(inputColor);
	parameters->ScalingOutputColor = builder.CreateUAV(outputColor);

	parameters->SamplerLinearClamp = RhiSamplerDesc{
	    .MinMagFilter = RhiSamplerMinMagFilter::Linear,
	    .MipFilter = RhiSamplerMipFilter::Linear,
	    .Address = MakeRhiSamplerAddressModes(RhiSamplerAddressMode::Clamp)};

	const std::uint32_t groupCountX = MathUtils::DivideRoundUp(outputExtent.Width, 8u);
	const std::uint32_t groupCountY = MathUtils::DivideRoundUp(outputExtent.Height, 8u);

	builder.Dispatch<LinearUpscaleCS>(parameters, ComputeDispatchDesc{groupCountX, groupCountY, 1u});
}
