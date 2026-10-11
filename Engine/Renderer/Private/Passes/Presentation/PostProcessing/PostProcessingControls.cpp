#include "../../../PCH.h"
#include "Passes/Presentation/PostProcessing/PostProcessingControls.h"

#include "Core/Public/Console/CVar.h"
#include "Passes/Presentation/PresentationPolicy.h"

static ConsoleVariable<bool> CVarPostProcessingDebugViews("r.PostProcessing.DebugViews", false, "Show post-processing effects in debug views. Lit and Reference use their enabled effect controls.");

bool IsPostProcessingShown(RenderViewMode viewMode) noexcept
{
	return !IsDiagnosticRenderView(viewMode) || CVarPostProcessingDebugViews.Get();
}
