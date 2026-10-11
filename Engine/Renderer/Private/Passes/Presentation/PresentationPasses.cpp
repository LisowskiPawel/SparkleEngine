#include "../../PCH.h"
#include "Passes/Presentation/PresentationPasses.h"

#include "Frame/RenderFrame.h"
#include "Frame/Graph/RenderFrameGraphResources.h"
#include "Passes/Presentation/PresentationPolicy.h"
#include "Passes/Presentation/Display/ToneMapping.h"
#include "Passes/Presentation/PostProcessing/Vignette.h"
#include "Passes/Presentation/PostProcessing/ChromaticAberration.h"
#include "Passes/Presentation/Display/OutputEncoding.h"
#include "Passes/Presentation/PresentationOutput.h"

void AddPresentationPasses(FrameGraphBuilder& builder, const RenderFrame& frame, const RenderFrameGraphSettings& settings, RenderFrameGraphResources& resources)
{
	if (!CanPublishPresentationOutput(resources))
	{
		return;
	}

	FrameGraphTextureHandle displayLinearColor = ResolveRenderViewPresentationDomain(frame.View.viewMode) == RenderViewPresentationDomain::DisplayLinearExact
	    ? resources.Presentation.ResolvedSceneColor
	    : AddToneMappingPass(builder, frame, settings.OutputExtent, resources);

	displayLinearColor = AddChromaticAberrationPass(builder, frame.View.viewMode, settings.OutputExtent, displayLinearColor);
	displayLinearColor = AddVignettePass(builder, frame.View.viewMode, settings.OutputExtent, displayLinearColor);
	const FrameGraphTextureHandle encodedColor = AddOutputEncodingPass(builder, settings, displayLinearColor);
	AddPresentationOutputPass(builder, settings, encodedColor, resources);
}
