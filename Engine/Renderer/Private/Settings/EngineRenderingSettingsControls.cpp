#include "PCH.h"
#include "Settings/EngineRenderingSettingsControls.h"

#include "Passes/Presentation/Display/OutputEncodingCVars.h"
#include "Passes/Presentation/PostProcessing/Vignette.h"
#include "Passes/Presentation/PostProcessing/ChromaticAberration.h"
#include "View/ViewportDisplayCVars.h"
#include "RayReconstruction/RayReconstructionSettings.h"
#include "Debug/RendererCVars.h"
#include "Renderer/Public/Settings/EngineRenderingSettings.h"
#include "RHI/Public/CVars/RHICVars.h"
#include "Upscaling/UpscalerSettings.h"
#include "Renderer/Public/ExternalCapture/ExternalCaptureStartup.h"
#include "RHI/Public/Diagnostics/RhiExternalCapture.h"

template <typename TState, typename TVisitor> static void VisitControlledSettings(TState& state, TVisitor&& visit) noexcept
{
	visit(CVarVSync, state.bVSync);
	visit(CVarVignette, state.bVignette);
	visit(CVarChromaticAberration, state.bChromaticAberration);
	visit(CVarBackBufferFormat, state.BackBufferFormat);
	visit(CVarPreferHighPerformanceAdapter, state.bPreferHighPerformanceAdapter);
	visit(CVarToneMapper, state.ToneMapper);
	visit(CVarExposureMode, state.ExposureMode);
	visit(CVarExposureMeteringMethod, state.ExposureMeteringMethod);
	visit(CVarOutputColorEncoding, state.OutputColorEncoding);
	visit(CVarManualExposure, state.ManualExposure);
	visit(CVarExposureCompensation, state.ExposureCompensation);
	visit(CVarExposureTargetLuminance, state.ExposureTargetLuminance);
	visit(CVarExposureMin, state.ExposureMin);
	visit(CVarExposureMax, state.ExposureMax);
	visit(CVarExposureAdaptationSpeedUp, state.ExposureAdaptationSpeedUp);
	visit(CVarExposureAdaptationSpeedDown, state.ExposureAdaptationSpeedDown);
	visit(CVarUpscalerProvider, state.UpscalerProvider);
	visit(CVarUpscalerQualityMode, state.UpscalerQualityMode);
	visit(CVarRayReconstructionMode, state.RayReconstructionMode);
	visit(CVarGBufferAlgorithm, state.SelectedGBufferAlgorithm);
	visit(CVarRendererMeshAutoBatching, state.bMeshAutoBatching);
	visit(CVarRayTracingTlasRefit, state.bRefitTlas);
	visit(CVarRayTracingPreferPtlas, state.bPtlasActive);
	visit(CVarRayTracingPartitionsPerAxis, state.PtlasPartitionsPerAxis);
	visit(CVarRayTracingPtlasPartitionUpdateMode, state.PtlasPartitionUpdateMode);
	visit(CVarRayTracingPtlasMarkAllDynamicInPartition, state.bPtlasMarkAllDynamicInPartition);
	visit(CVarRayTracingPtlasModeChangeDistance, state.PtlasModeChangeDistance);
#if SPARKLE_WITH_EXTERNAL_CAPTURE
	visit(CVarExternalCaptureStartupProvider, state.StartupCaptureProvider);
#endif
}

EngineRenderingSettingsState CaptureEngineRenderingSettings() noexcept
{
	EngineRenderingSettingsState state;
	VisitControlledSettings(state, [](const auto& cvar, auto& value) noexcept { value = cvar.Get(); });
#if SPARKLE_WITH_EXTERNAL_CAPTURE
	state.bCaptureToolsInstalled = HasInstalledExternalCaptureProvider();
#endif
	return state;
}

void ApplyEngineRenderingSettings(const EngineRenderingSettingsState& state) noexcept
{
	VisitControlledSettings(
	    state,
	    [](auto& cvar, const auto& value) noexcept
	    {
		    if (cvar.Get() != value)
		    {
			    cvar.Set(value);
		    }
	    });
}
