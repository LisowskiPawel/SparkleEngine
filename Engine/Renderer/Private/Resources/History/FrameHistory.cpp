#include "PCH.h"
#include "Resources/History/FrameHistory.h"

#include "FrameGraph/Builder/FrameGraphBuilder.h"
#include "FrameGraph/FrameGraph.h"
#include "FrameGraph/FrameGraphTextureDesc.h"
#include "RHI/Public/Formats/PixelFormat.h"
#include "View/RenderView.h"

#include <string>

void InvalidateReservoirHistory(FrameGraph& frameGraph, const FrameGraphReservoirHistoryHandles& history) noexcept
{
	frameGraph.InvalidateTextureHistory(history.Sample);
	frameGraph.InvalidateTextureHistory(history.Weight);
	frameGraph.InvalidateTextureHistory(history.Surface);
}

FrameHistoryResourceLayout DeclareFrameHistoryResources(FrameGraphBuilder& builder)
{
	return FrameHistoryResourceLayout{.Exposure = builder.CreateTextureHistory(FrameGraphTextureDesc::CreateColor("Exposure", 1u, 1u, PixelFormat::R32G32B32A32_Float))};
}

FrameGraphReservoirHistoryHandles DeclareLightingReservoirHistory(FrameGraphBuilder& builder, RenderViewportExtent renderExtent, std::string_view name)
{
	const auto declare = [&](std::string_view suffix, PixelFormat format)
	{
		return builder.CreateTextureHistory(FrameGraphTextureDesc::CreateColor(std::string(name) + std::string(suffix), renderExtent.Width, renderExtent.Height, format));
	};

	return FrameGraphReservoirHistoryHandles{
	    .Sample = declare("Sample", PixelFormat::R32G32B32A32_Float),
	    .Weight = declare("Weight", PixelFormat::R32G32B32A32_Float),
	    .Surface = declare("Surface", PixelFormat::R16G16B16A16_Float)};
}

void InvalidateFrameHistory(FrameGraph& frameGraph, const FrameHistoryResourceLayout& history) noexcept
{
	frameGraph.InvalidateTextureHistory(history.Exposure);
	InvalidateReservoirHistory(frameGraph, history.DirectLightReservoir);
	InvalidateReservoirHistory(frameGraph, history.RestirIndirectReservoir);
}

void UpdateFrameHistory(FrameGraph& frameGraph, const FrameHistoryResourceLayout& history, const RenderView& view)
{
	if (view.temporalUniform.HistoryValid == 0u)
	{
		InvalidateFrameHistory(frameGraph, history);
	}
}
