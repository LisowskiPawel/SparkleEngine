#include "PCH.h"
#include "Passes/Lighting/Direct/DirectLightReservoirResources.h"

#include "Frame/Graph/RenderFrameGraphResources.h"
#include "FrameGraph/Builder/FrameGraphBuilder.h"
#include "FrameGraph/FrameGraphTextureDesc.h"
#include "Resources/History/FrameHistory.h"
#include "RHI/Public/Formats/PixelFormat.h"

void CreateDirectLightReservoirResources(FrameGraphBuilder& builder, RenderViewportExtent extent, RenderFrameGraphResources& resources)
{
	resources.History.DirectLightReservoir = DeclareLightingReservoirHistory(builder, extent, "DirectLightReservoir");

	resources.Transient.DirectLightTemporalReservoirSample = builder.CreateTexture(
	    FrameGraphTextureDesc::CreateColor("DirectLightTemporalReservoirSample", extent.Width, extent.Height, PixelFormat::R32G32B32A32_Float));

	resources.Transient.DirectLightTemporalReservoirWeight = builder.CreateTexture(
	    FrameGraphTextureDesc::CreateColor("DirectLightTemporalReservoirWeight", extent.Width, extent.Height, PixelFormat::R32G32B32A32_Float));
}
