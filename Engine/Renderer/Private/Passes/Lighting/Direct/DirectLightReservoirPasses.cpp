#include "../../../PCH.h"
#include "Passes/Lighting/Direct/DirectLightReservoirPasses.h"

#include "Frame/RenderFrame.h"
#include "Passes/Lighting/Direct/DirectLightReservoirPassDefinitions.h"
#include "Passes/Lighting/Direct/DirectLightReservoirResources.h"

void AddDirectLightReservoirPasses(FrameGraphBuilder& builder, const RenderFrame& frame, RenderViewportExtent sceneExtent, RenderFrameGraphResources& resources)
{
	CreateDirectLightReservoirResources(builder, sceneExtent, resources);

	AddDirectLightReservoirTemporalPass(builder, frame, sceneExtent, resources);
	AddDirectLightReservoirSpatialPass(builder, frame, sceneExtent, resources);
}
