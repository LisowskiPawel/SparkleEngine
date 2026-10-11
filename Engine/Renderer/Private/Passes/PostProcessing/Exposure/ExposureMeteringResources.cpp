#include "../../../PCH.h"
#include "Passes/PostProcessing/Exposure/ExposureMeteringResources.h"

#include "FrameGraph/Builder/FrameGraphBuilder.h"
#include "FrameGraph/FrameGraphTextureDesc.h"
#include "FrameGraph/FrameGraphBufferDesc.h"
#include "RHI/Public/Formats/PixelFormat.h"

#include <string>

FrameGraphBufferHandle CreateExposureHistogramBuffer(FrameGraphBuilder& builder)
{
	// Matches the fixed bin layout in ExposureHistogram.hlsli.
	constexpr std::uint32_t histogramBinCount = 512u;
	return builder.CreateBuffer(FrameGraphBufferDesc::Create("ExposureHistogram", histogramBinCount * sizeof(std::uint32_t), sizeof(std::uint32_t)));
}

ExposureMomentTexture CreateExposureMomentTexture(FrameGraphBuilder& builder, const char* prefix, std::uint32_t level, std::uint32_t width, std::uint32_t height)
{
	const std::string name = std::string(prefix) + std::to_string(level);
	return ExposureMomentTexture{.Handle = builder.CreateTexture(FrameGraphTextureDesc::CreateColor(name, width, height, PixelFormat::R32G32B32A32_Float)), .Width = width, .Height = height};
}
