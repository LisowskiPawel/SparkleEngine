#include "../../PCH.h"
#include "Passes/Core/ComputeClear.h"

#include "Core/Public/Math/MathUtils.h"
#include "FrameGraph/Builder/FrameGraphBuilder.h"

void AddComputeClearPass(FrameGraphBuilder& builder, std::string_view label, FrameGraphTextureHandle outputTexture, RenderViewportExtent outputExtent)
{
	auto& parameters = builder.AllocParameters<ComputeClearCS>();
	parameters->Output = builder.CreateUAV(outputTexture);

	const std::uint32_t groupCountX = MathUtils::DivideRoundUp(outputExtent.Width, 8u);
	const std::uint32_t groupCountY = MathUtils::DivideRoundUp(outputExtent.Height, 8u);

	builder.Dispatch<ComputeClearCS>(label, parameters, ComputeDispatchDesc{groupCountX, groupCountY, 1u});
}
