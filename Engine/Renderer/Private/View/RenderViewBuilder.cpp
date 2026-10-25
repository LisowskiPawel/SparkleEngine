#include "PCH.h"

#include "View/RenderViewBuilder.h"

#include "View/RenderView.h"
#include "View/RenderViewState.h"
#include "View/RenderViewProjection.h"

void BuildRenderView(RenderView& output, RenderViewState& state, const RenderViewBuildRequest& request) noexcept
{
	output.InitializeFromRequest(request);

	BuildRenderViewCamera(output, request.ViewportRequest.Extent.IsValid() ? request.ViewportRequest.Extent : request.OutputExtent);
	BuildRenderViewRasterState(output, request.RenderExtent);

	state.UpdateTemporal(output, request);
}
