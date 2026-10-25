#include "PCH.h"

#include "View/RenderView.h"
#include "View/RenderViewBuilder.h"

void RenderView::ResetForReuse() noexcept
{
	std::vector<std::uint32_t> retainedRasterPrimitiveIndices = std::move(rasterPrimitiveIndices);
	std::vector<MeshInstanceBatch> retainedMeshInstanceBatches = std::move(meshInstanceBatches);
	*this = {};
	rasterPrimitiveIndices = std::move(retainedRasterPrimitiveIndices);
	meshInstanceBatches = std::move(retainedMeshInstanceBatches);
	rasterPrimitiveIndices.clear();
	meshInstanceBatches.clear();
}

void RenderView::InitializeFromRequest(const RenderViewBuildRequest& request) noexcept
{
	ResetForReuse();
	viewportId = request.ViewportRequest.ViewportId;
	selection = request.ViewportRequest.ViewSelection;
	kind = request.ViewportRequest.ViewKind;
	viewMode = request.ViewportRequest.ViewMode;
	renderExtent = request.RenderExtent;
	outputExtent = request.OutputExtent;
	displaySettings = ResolvedViewportDisplaySettings::Resolve(request.ViewportRequest.Exposure);
	camera = request.Input.Camera;

}
