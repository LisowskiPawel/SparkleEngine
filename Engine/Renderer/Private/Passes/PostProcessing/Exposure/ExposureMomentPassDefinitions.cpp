#include "../../../PCH.h"
#include "Passes/PostProcessing/Exposure/ExposureMomentPassDefinitions.h"

#include "Core/Public/Math/MathUtils.h"
#include "FrameGraph/Builder/FrameGraphBuilder.h"
#include "Passes/PostProcessing/Exposure/ExposureDownsampleSceneShader.h"
#include "Passes/PostProcessing/Exposure/ExposureDownsampleTextureShader.h"

void AddExposureSceneDownsamplePass(FrameGraphBuilder& builder, FrameGraphTextureHandle sceneColor, const ExposureMomentTexture& output)
{
	auto& parameters = builder.AllocParameters<ExposureDownsampleSceneCS>();
	parameters->SceneColor = builder.CreateSRV(sceneColor);
	parameters->LuminanceMomentsOutput = builder.CreateUAV(output.Handle);

	const std::uint32_t groupCountX = MathUtils::DivideRoundUp(output.Width, 8u);
	const std::uint32_t groupCountY = MathUtils::DivideRoundUp(output.Height, 8u);

	builder.Dispatch<ExposureDownsampleSceneCS>(parameters, ComputeDispatchDesc{groupCountX, groupCountY, 1u}, EFrameGraphQueuePreference::AsyncCompute);
}

void AddExposureTextureDownsamplePass(FrameGraphBuilder& builder, const ExposureMomentTexture& input, const ExposureMomentTexture& output)
{
	auto& parameters = builder.AllocParameters<ExposureDownsampleTextureCS>();
	parameters->LuminanceMomentsInput = builder.CreateSRV(input.Handle);
	parameters->LuminanceMomentsOutput = builder.CreateUAV(output.Handle);

	const std::uint32_t groupCountX = MathUtils::DivideRoundUp(output.Width, 8u);
	const std::uint32_t groupCountY = MathUtils::DivideRoundUp(output.Height, 8u);

	builder.Dispatch<ExposureDownsampleTextureCS>(parameters, ComputeDispatchDesc{groupCountX, groupCountY, 1u}, EFrameGraphQueuePreference::AsyncCompute);
}
