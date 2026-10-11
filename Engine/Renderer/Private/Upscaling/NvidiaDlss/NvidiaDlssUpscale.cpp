#include "../../PCH.h"
#include "Upscaling/NvidiaDlss/NvidiaDlssUpscale.h"

#include "Providers/RendererImageProviderStack.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Frame/Graph/RenderFrameGraphResources.h"
#include "Upscaling/UpscalerPass.h"

#include <string>

static void RequireDlssInput(FrameGraphTextureHandle input, const char* name)
{
	if (!input.IsValid())
	{
		throw Diagnostics::Error(std::string("NVIDIA DLSS requires a valid ") + name + " input.");
	}
}

void AddNvidiaDlssUpscalePass(
    FrameGraphBuilder& builder,
    RenderViewportExtent renderExtent,
    RenderViewportExtent outputExtent,
    RendererImageProviderStack& imageProviders,
    const RenderFrameGraphResources& resources)
{
	const UpscalerPassResources inputs{
	    .InputColor = resources.Presentation.SceneColorInput,
	    .OutputColor = resources.Presentation.ResolvedSceneColor,
	    .Depth = resources.Transient.GBuffer.DeviceZ,
	    .MotionVectors = resources.Transient.GBuffer.MotionVector,
	    .Exposure = resources.Transient.Exposure};

	RequireDlssInput(inputs.InputColor, "scene color");
	RequireDlssInput(inputs.OutputColor, "resolved color");
	RequireDlssInput(inputs.Depth, "device depth");
	RequireDlssInput(inputs.MotionVectors, "motion vectors");
	RequireDlssInput(inputs.Exposure, "exposure");
	if (imageProviders.GetUpscalerProvider() == nullptr)
	{
		throw Diagnostics::Error("NVIDIA DLSS graph construction has no initialized upscaler provider.");
	}

	AddUpscalerPass(builder, *imageProviders.GetUpscalerProvider(), renderExtent, outputExtent, inputs);
}
