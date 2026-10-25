#include "PCH.h"

#include "View/RenderViewBuilder.h"

#include "Renderer/Public/Viewport/ViewportContracts.h"
#include "View/RenderView.h"
#include "View/RenderViewState.h"

#include "View/RenderViewProjection.h"

void BuildRenderView(RenderView& output, RenderViewState& state, const RenderViewBuildRequest& request) noexcept
{
	output.ResetForReuse();
	output.viewportId = request.ViewportRequest.ViewportId;
	output.selection = request.ViewportRequest.ViewSelection;
	output.kind = request.ViewportRequest.ViewKind;
	output.viewMode = request.ViewportRequest.ViewMode;
	output.renderExtent = request.RenderExtent;
	output.outputExtent = request.OutputExtent;
	output.displaySettings = ResolvedViewportDisplaySettings::Resolve(request.ViewportRequest.Exposure);
	output.camera = request.Input.Camera;

	BuildRenderViewCamera(output, request.ViewportRequest.Extent.IsValid() ? request.ViewportRequest.Extent : request.OutputExtent);
	BuildRenderViewRasterState(output, request.RenderExtent);

	output.temporalUniform = state.BuildTemporal(
	    RenderViewStateBuildInput{
	        .ViewInput = request.Input,
	        .Identity =
	            {
	                .ViewportId = output.viewportId,
	                .Selection = output.selection.Value,
	                .Kind = output.kind,
	                .Mode = output.viewMode,
	            },
	        .Camera = output.cameraUniform,
	        .RenderExtent = request.RenderExtent,
	        .FrameId = request.FrameId,
	        .SceneGeneration = request.SceneGeneration,
	        .ShaderGeneration = request.ShaderGeneration,
	        .ImageProviderGeneration = request.ImageProviderGeneration,
	        .GraphTopologyGeneration = request.GraphTopologyGeneration});
}
