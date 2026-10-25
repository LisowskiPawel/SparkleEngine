#include "PCH.h"
#include "Passes/Lighting/Restir/Indirect/RestirIndirectLightingPasses.h"

#include "Frame/RenderFrame.h"
#include "Passes/Lighting/Restir/Indirect/RestirIndirectReservoirPasses.h"
#include "Passes/Lighting/Restir/Indirect/RestirIndirectResolve.h"
#include "Passes/Lighting/Restir/Indirect/IndirectLightingControls.h"
#include "Passes/Lighting/LightingRenderTargets.h"
#include "Passes/Lighting/LightingTargetClear.h"
#include "Passes/Lighting/Restir/Reconstruction/RayReconstructionSurfaceGuides.h"
#include "Passes/Lighting/Restir/Reconstruction/RestirRayReconstructionResources.h"

void AddRestirIndirectLightingPasses(FrameGraphBuilder& builder, const RenderFrame& frame, RenderViewportExtent sceneExtent, RenderFrameGraphResources& resources)
{
	CreateIndirectLightingRenderTargets(builder, sceneExtent, resources);
	AddIndirectLightingClearPass(builder, sceneExtent, resources);
	CreateRayReconstructionGuideRenderTargets(builder, sceneExtent, resources);
	AddRayReconstructionGuideClearPass(builder, sceneExtent, resources);
	AddRayReconstructionSurfaceGuidesPass(builder, frame, sceneExtent, resources);

	if (!IsIndirectLightingAdmitted())
	{
		return;
	}

	AddRestirIndirectReservoirPasses(builder, frame, sceneExtent, resources);
	AddRestirIndirectResolvePass(builder, frame, sceneExtent, resources);
}
