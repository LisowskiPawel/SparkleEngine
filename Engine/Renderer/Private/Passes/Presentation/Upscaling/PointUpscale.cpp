#include "../../../PCH.h"
#include "Passes/Presentation/Upscaling/PointUpscale.h"

#include "Core/Public/Math/MathUtils.h"
#include "FrameGraph/Builder/FrameGraphBuilder.h"
#include "Passes/Presentation/Upscaling/PointUpscaleShader.h"

void AddPointUpscalePass(FrameGraphBuilder& builder, FrameGraphTextureHandle inputColor, FrameGraphTextureHandle outputColor, RenderViewportExtent outputExtent)
{
	auto& parameters = builder.AllocParameters<PointUpscaleCS>();
	parameters->ScalingInputColor = builder.CreateSRV(inputColor);
	parameters->ScalingOutputColor = builder.CreateUAV(outputColor);

	const std::uint32_t groupCountX = MathUtils::DivideRoundUp(outputExtent.Width, 8u);
	const std::uint32_t groupCountY = MathUtils::DivideRoundUp(outputExtent.Height, 8u);

	builder.Dispatch<PointUpscaleCS>(parameters, ComputeDispatchDesc{groupCountX, groupCountY, 1u});
}
