#include "PCH.h"
#include "Frame/Graph/ViewportFrameProductExports.h"

#include "Frame/Graph/RenderFrameGraphResources.h"
#include "Frame/Graph/RenderFrameGraphSettings.h"
#include "FrameGraph/Builder/FrameGraphBuilder.h"

void ExportViewportFrameProducts(FrameGraphBuilder& builder, const RenderFrameGraphSettings& settings, const RenderFrameGraphResources& resources) noexcept
{
	if (resources.ViewportProducts.FinalColorLdr.IsValid())
	{
		builder.ExportTexture(resources.ViewportProducts.FinalColorLdr, "Viewport.FinalColorLdr");
	}

	const bool exportSceneDepth = HasAnyRenderOutputFlags(settings.RequestedOutputs, RenderOutputFlags::SceneDepth) && resources.ViewportProducts.SceneDepth.IsValid();
	
	if (exportSceneDepth)
	{
		builder.ExportTexture(resources.ViewportProducts.SceneDepth, "Viewport.SceneDepth");
	}

	const bool exportNormals = HasAnyRenderOutputFlags(settings.RequestedOutputs, RenderOutputFlags::Normals) && resources.ViewportProducts.Normals.IsValid();
	
	if (exportNormals)
	{
		builder.ExportTexture(resources.ViewportProducts.Normals, "Viewport.Normals");
	}
}
