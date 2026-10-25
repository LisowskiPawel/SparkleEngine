#include "../../PCH.h"
#include "Passes/Lighting/LightingTargetClear.h"

#include "Frame/Graph/RenderFrameGraphResources.h"
#include "FrameGraph/Builder/FrameGraphBuilder.h"
#include "FrameGraph/Execution/PassCommandContext.h"
#include "FrameGraph/ResourceUsage.h"
#include "Renderer/Public/Viewport/ViewportContracts.h"

#include <array>
#include <vector>

void AddLightingTargetClearPass(FrameGraphBuilder& builder, std::string_view name, RenderViewportExtent extent, std::span<const FrameGraphTextureHandle> targets)
{
	const std::vector<FrameGraphTextureHandle> ownedTargets(targets.begin(), targets.end());

	builder.AddPass(
	    name,
	    EFrameGraphPassKind::Raster,
	    [ownedTargets](PassResourceBuilder& resources)
	    {
		    for (const auto target : ownedTargets)
		    {
			    resources.Write(target, ResourceUsage::RenderTarget, "LightingTarget");
		    }
	    },
	    [ownedTargets, extent](PassCommandContext& context)
	    {
		    context.Commands.SetScissorRect(0, 0, static_cast<std::int32_t>(extent.Width), static_cast<std::int32_t>(extent.Height));
		    context.Resources.BindRenderTargets(context.Commands, ownedTargets);
		    for (const auto target : ownedTargets)
		    {
			    context.Resources.ClearRenderTarget(context.Commands, target);
		    }
		    context.Resources.EndRasterPass(context.Commands);
	    });
}

void AddDirectLightingClearPass(FrameGraphBuilder& builder, RenderViewportExtent extent, const RenderFrameGraphResources& resources)
{
	const auto& lighting = resources.Transient.Lighting;
	const std::array targets{lighting.DirectDiffuse, lighting.DirectSpecular, lighting.DirectSubsurface};
	AddLightingTargetClearPass(builder, "DirectLightingTargetClear", extent, targets);
}

void AddIndirectLightingClearPass(FrameGraphBuilder& builder, RenderViewportExtent extent, const RenderFrameGraphResources& resources)
{
	const auto& lighting = resources.Transient.Lighting;
	const std::array targets{lighting.IndirectDiffuse, lighting.IndirectSpecular};
	AddLightingTargetClearPass(builder, "IndirectLightingTargetClear", extent, targets);
}

void AddRayReconstructionGuideClearPass(FrameGraphBuilder& builder, RenderViewportExtent extent, const RenderFrameGraphResources& resources)
{
	const auto& guides = resources.Transient.Lighting.ReconstructionGuides;
	const std::array targets{guides.DiffuseAlbedo, guides.SpecularAlbedo, guides.Roughness, guides.SpecularHitDistance};
	AddLightingTargetClearPass(builder, "RayReconstructionGuideClear", extent, targets);
}
