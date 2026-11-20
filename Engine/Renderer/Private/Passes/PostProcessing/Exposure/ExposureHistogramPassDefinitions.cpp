#include "../../../PCH.h"
#include "Passes/PostProcessing/Exposure/ExposureHistogramPassDefinitions.h"

#include "Core/Public/Math/MathUtils.h"
#include "FrameGraph/Builder/FrameGraphBuilder.h"
#include "Passes/PostProcessing/Exposure/ExposureHistogramShaders.h"
#include "Renderer/Public/Viewport/ViewportContracts.h"

void AddExposureHistogramClearPass(FrameGraphBuilder& builder, FrameGraphBufferHandle histogram)
{
	auto& parameters = builder.AllocParameters<ExposureHistogramClearCS>();
	parameters->HistogramCounts = builder.CreateUAV(histogram);
	builder.Dispatch<ExposureHistogramClearCS>(parameters, ComputeDispatchDesc{1u, 1u, 1u}, EFrameGraphQueuePreference::AsyncCompute);
}

void AddExposureHistogramBuildPass(FrameGraphBuilder& builder, FrameGraphTextureHandle sceneColor, RenderViewportExtent extent, FrameGraphBufferHandle histogram)
{
	auto& parameters = builder.AllocParameters<ExposureHistogramBuildCS>();
	parameters->SceneColor = builder.CreateSRV(sceneColor);
	parameters->HistogramCounts = builder.CreateUAV(histogram);

	const std::uint32_t groupCountX = MathUtils::DivideRoundUp(extent.Width, 16u);
	const std::uint32_t groupCountY = MathUtils::DivideRoundUp(extent.Height, 16u);

	builder.Dispatch<ExposureHistogramBuildCS>(parameters, ComputeDispatchDesc{groupCountX, groupCountY, 1u}, EFrameGraphQueuePreference::AsyncCompute);
}

void AddExposureHistogramResolvePass(FrameGraphBuilder& builder, FrameGraphBufferHandle histogram, const ExposureMomentTexture& output)
{
	auto& parameters = builder.AllocParameters<ExposureHistogramResolveCS>();
	parameters->HistogramCounts = builder.CreateSRV(histogram);
	parameters->LuminanceMomentsOutput = builder.CreateUAV(output.Handle);
	builder.Dispatch<ExposureHistogramResolveCS>(parameters, ComputeDispatchDesc{1u, 1u, 1u}, EFrameGraphQueuePreference::AsyncCompute);
}
