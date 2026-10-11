#include "PCH.h"
#include "Passes/Lighting/Restir/Indirect/IndirectLightingResources.h"

#include "Frame/Graph/RenderFrameGraphResources.h"
#include "Passes/Lighting/LightingRenderTargets.h"
#include "Passes/Lighting/LightingTargetClear.h"

#include <array>

void CreateIndirectLightingResources(FrameGraphBuilder& builder, RenderViewportExtent sceneExtent, RenderFrameGraphResources& resources)
{
	CreateIndirectLightingRenderTargets(builder, sceneExtent, resources);
	const auto& lighting = resources.Transient.Lighting;
	const std::array indirectTargets{lighting.IndirectDiffuse, lighting.IndirectSpecular};
	AddLightingTargetClearPass(builder, "IndirectLightingTargetClear", sceneExtent, indirectTargets);
}
