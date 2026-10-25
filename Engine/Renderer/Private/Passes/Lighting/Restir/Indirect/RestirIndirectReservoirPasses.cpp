#include "PCH.h"
#include "Passes/Lighting/Restir/Indirect/RestirIndirectReservoirPasses.h"

#include "Frame/Graph/RenderFrameGraphResources.h"
#include "Passes/Lighting/Restir/Indirect/RestirIndirectReservoirResources.h"
#include "Passes/Lighting/Restir/Indirect/RestirIndirectTemporal.h"
#include "Passes/Lighting/Restir/Indirect/RestirIndirectSpatial.h"
#include "Passes/Core/ComputeClear.h"
#include "Resources/History/FrameHistory.h"

void AddRestirIndirectReservoirPasses(FrameGraphBuilder& builder, const RenderFrame& frame, RenderViewportExtent sceneExtent, RenderFrameGraphResources& resources)
{
	CreateRestirIndirectHistoryResources(builder, sceneExtent, resources);
	const RestirIndirectWorkingReservoirs workingReservoirs = CreateRestirIndirectWorkingReservoirs(builder, sceneExtent);
	const FrameGraphReservoirHistoryHandles& history = resources.History.RestirIndirectReservoir;

	AddComputeClearPass(builder, "ClearRestirIndirectTemporalSample", workingReservoirs.TemporalSample, sceneExtent);
	AddComputeClearPass(builder, "ClearRestirIndirectTemporalWeight", workingReservoirs.TemporalWeight, sceneExtent);
	AddComputeClearPass(builder, "ClearRestirIndirectCurrentSample", history.Sample.Current, sceneExtent);
	AddComputeClearPass(builder, "ClearRestirIndirectCurrentWeight", history.Weight.Current, sceneExtent);
	AddComputeClearPass(builder, "ClearRestirIndirectCurrentSurface", history.Surface.Current, sceneExtent);

	AddRestirIndirectTemporalPass(builder, frame, sceneExtent, workingReservoirs, resources);
	AddRestirIndirectSpatialPass(builder, frame, sceneExtent, workingReservoirs, resources);
}
