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
	const FrameGraphBufferDesc desc = FrameGraphBufferDesc::Create("ExposureHistogram", histogramBinCount * sizeof(std::uint32_t), sizeof(std::uint32_t));
	return builder.CreateBuffer(desc);
}

ExposureMomentTexture CreateExposureMomentTexture(FrameGraphBuilder& builder, const char* prefix, std::uint32_t level, std::uint32_t width, std::uint32_t height)
{
	const std::string name = std::string(prefix) + std::to_string(level);
	const FrameGraphTextureDesc desc = FrameGraphTextureDesc::CreateColor(name, width, height, PixelFormat::R32G32B32A32_Float);
	const FrameGraphTextureHandle handle = builder.CreateTexture(desc);
	return ExposureMomentTexture{.Handle = handle, .Width = width, .Height = height};
}
