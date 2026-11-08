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

EngineRenderingSettingsState CaptureEngineRenderingSettings() noexcept
{
	EngineRenderingSettingsState state;
	state.bVSync = CVarVSync.Get();
	state.bVignette = CVarVignette.Get();
	state.bChromaticAberration = CVarChromaticAberration.Get();
	state.BackBufferFormat = CVarBackBufferFormat.Get();
	state.bPreferHighPerformanceAdapter = CVarPreferHighPerformanceAdapter.Get();
	state.ToneMapper = CVarToneMapper.Get();
	state.ExposureMode = CVarExposureMode.Get();
	state.ExposureMeteringMethod = CVarExposureMeteringMethod.Get();
	state.OutputColorEncoding = CVarOutputColorEncoding.Get();
	state.ManualExposure = CVarManualExposure.Get();
	state.ExposureCompensation = CVarExposureCompensation.Get();
	state.ExposureTargetLuminance = CVarExposureTargetLuminance.Get();
	state.ExposureMin = CVarExposureMin.Get();
	state.ExposureMax = CVarExposureMax.Get();
	state.ExposureAdaptationSpeedUp = CVarExposureAdaptationSpeedUp.Get();
	state.ExposureAdaptationSpeedDown = CVarExposureAdaptationSpeedDown.Get();
	state.UpscalerProvider = CVarUpscalerProvider.Get();
	state.UpscalerQualityMode = CVarUpscalerQualityMode.Get();
	state.RayReconstructionMode = CVarRayReconstructionMode.Get();
	state.SelectedGBufferAlgorithm = CVarGBufferAlgorithm.Get();
	state.bMeshAutoBatching = CVarRendererMeshAutoBatching.Get();
	state.bRefitTlas = CVarRayTracingTlasRefit.Get();
	state.bPtlasActive = CVarRayTracingPreferPtlas.Get();
	state.PtlasPartitionsPerAxis = CVarRayTracingPartitionsPerAxis.Get();
	state.PtlasPartitionUpdateMode = CVarRayTracingPtlasPartitionUpdateMode.Get();
	state.bPtlasMarkAllDynamicInPartition = CVarRayTracingPtlasMarkAllDynamicInPartition.Get();
	state.PtlasModeChangeDistance = CVarRayTracingPtlasModeChangeDistance.Get();
#if SPARKLE_WITH_EXTERNAL_CAPTURE
	state.StartupCaptureProvider = CVarExternalCaptureStartupProvider.Get();
	state.bCaptureToolsInstalled = HasInstalledExternalCaptureProvider();
#endif
	return state;
}

void ApplyEngineRenderingSettings(const EngineRenderingSettingsState& state) noexcept
{
	const auto setCVarIfChanged = []<typename TCVar, typename TValue>(TCVar& cvar, const TValue& value) noexcept
	{
		if (cvar.Get() != value)
		{
			cvar.Set(value);
		}
	};

	setCVarIfChanged(CVarVSync, state.bVSync);
	setCVarIfChanged(CVarVignette, state.bVignette);
	setCVarIfChanged(CVarChromaticAberration, state.bChromaticAberration);
	setCVarIfChanged(CVarBackBufferFormat, state.BackBufferFormat);
	setCVarIfChanged(CVarPreferHighPerformanceAdapter, state.bPreferHighPerformanceAdapter);
	setCVarIfChanged(CVarToneMapper, state.ToneMapper);
	setCVarIfChanged(CVarExposureMode, state.ExposureMode);
	setCVarIfChanged(CVarExposureMeteringMethod, state.ExposureMeteringMethod);
	setCVarIfChanged(CVarOutputColorEncoding, state.OutputColorEncoding);
	setCVarIfChanged(CVarManualExposure, state.ManualExposure);
	setCVarIfChanged(CVarExposureCompensation, state.ExposureCompensation);
	setCVarIfChanged(CVarExposureTargetLuminance, state.ExposureTargetLuminance);
	setCVarIfChanged(CVarExposureMin, state.ExposureMin);
	setCVarIfChanged(CVarExposureMax, state.ExposureMax);
	setCVarIfChanged(CVarExposureAdaptationSpeedUp, state.ExposureAdaptationSpeedUp);
	setCVarIfChanged(CVarExposureAdaptationSpeedDown, state.ExposureAdaptationSpeedDown);
	setCVarIfChanged(CVarUpscalerProvider, state.UpscalerProvider);
	setCVarIfChanged(CVarUpscalerQualityMode, state.UpscalerQualityMode);
	setCVarIfChanged(CVarRayReconstructionMode, state.RayReconstructionMode);
	setCVarIfChanged(CVarGBufferAlgorithm, state.SelectedGBufferAlgorithm);
	setCVarIfChanged(CVarRendererMeshAutoBatching, state.bMeshAutoBatching);
	setCVarIfChanged(CVarRayTracingTlasRefit, state.bRefitTlas);
	setCVarIfChanged(CVarRayTracingPreferPtlas, state.bPtlasActive);
	setCVarIfChanged(CVarRayTracingPartitionsPerAxis, state.PtlasPartitionsPerAxis);
	setCVarIfChanged(CVarRayTracingPtlasPartitionUpdateMode, state.PtlasPartitionUpdateMode);
	setCVarIfChanged(CVarRayTracingPtlasMarkAllDynamicInPartition, state.bPtlasMarkAllDynamicInPartition);
	setCVarIfChanged(CVarRayTracingPtlasModeChangeDistance, state.PtlasModeChangeDistance);
#if SPARKLE_WITH_EXTERNAL_CAPTURE
	setCVarIfChanged(CVarExternalCaptureStartupProvider, state.StartupCaptureProvider);
#endif
}
