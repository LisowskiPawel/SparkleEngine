#include "../../PCH.h"
#include "Frame/Graph/RenderFrameGraphResources.h"

#include "Frame/Graph/RenderFrameGraphFormats.h"
#include "Frame/Graph/RenderFrameGraphSettings.h"
#include "FrameGraph/Builder/FrameGraphBuilder.h"
#include "FrameGraph/FrameGraphTextureDesc.h"
#include "RHI/Public/Interop/ResourceState.h"

RenderFrameGraphResources CreateRenderFrameGraphResources(FrameGraphBuilder& builder, const RenderFrameGraphSettings& settings)
{
	RenderFrameGraphResources resources;

	const FrameGraphTextureDesc sceneColorDesc = FrameGraphTextureDesc::CreateColor("SceneColor", settings.RenderExtent.Width, settings.RenderExtent.Height, RenderFrameGraphFormats::SceneColor);
	resources.Transient.Scene.SceneColor = builder.CreateTexture(sceneColorDesc);

	resources.Presentation.SceneColorInput = resources.Transient.Scene.SceneColor;

	const FrameGraphTextureDesc sceneDepthDesc = FrameGraphTextureDesc::CreateColor("SceneDepth", settings.RenderExtent.Width, settings.RenderExtent.Height, RenderFrameGraphFormats::SceneDepth);
	resources.Transient.Scene.SceneDepth = builder.CreateTexture(sceneDepthDesc);

	const FrameGraphTextureDesc exposureDesc = FrameGraphTextureDesc::CreateColor("Exposure", 1, 1, PixelFormat::R32G32B32A32_Float);
	resources.Transient.Exposure = builder.CreateTexture(exposureDesc);

	const FrameGraphTextureDesc skyDesc = FrameGraphTextureDesc::CreateColor("Sky", 1, 1, PixelFormat::R8G8B8A8_UNorm);
	resources.ImportedScene.Sky = builder.ReservePersistentTexture(skyDesc, ResourceState::ShaderResource);

	resources.ImportedScene.Scene = DeclareRenderSceneGpuResources(builder);
	resources.History = DeclareFrameHistoryResources(builder);

	if (settings.PresentationTarget == FramePresentationTarget::BackBuffer)
	{
		const FrameGraphTextureDesc backBufferDesc = FrameGraphTextureDesc::CreateColor("BackBuffer", settings.OutputExtent.Width, settings.OutputExtent.Height, settings.OutputFormat);
		resources.Presentation.BackBuffer = builder.ImportBackBuffer(backBufferDesc, ResourceState::Present);
	}

	resources.ViewportProducts.SceneDepth = resources.Transient.Scene.SceneDepth;
	return resources;
}

FrameGraphTextureHandle CreateResolvedSceneColorTarget(FrameGraphBuilder& builder, RenderViewportExtent outputExtent)
{
	const FrameGraphTextureDesc desc = FrameGraphTextureDesc::CreateColor("ResolvedSceneColor", outputExtent.Width, outputExtent.Height, RenderFrameGraphFormats::SceneColor);
	return builder.CreateTexture(desc);
}
