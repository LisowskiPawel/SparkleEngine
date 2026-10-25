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

EngineRenderingSettingsState EngineRenderingSettingsControls::Capture() noexcept
{
	EngineRenderingSettingsState state;
	state.VSync = CVarVSync.Get();
	state.Vignette = CVarVignette.Get();
	state.ChromaticAberration = CVarChromaticAberration.Get();
	state.BackBufferFormat = CVarBackBufferFormat.Get();
	state.PreferHighPerformanceAdapter = CVarPreferHighPerformanceAdapter.Get();
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
	state.MeshAutoBatching = CVarRendererMeshAutoBatching.Get();
	state.RefitTlas = CVarRayTracingTlasRefit.Get();
	state.PtlasActive = CVarRayTracingPreferPtlas.Get();
	state.PtlasPartitionsPerAxis = CVarRayTracingPartitionsPerAxis.Get();
	state.PtlasPartitionUpdateMode = CVarRayTracingPtlasPartitionUpdateMode.Get();
	state.PtlasMarkAllDynamicInPartition = CVarRayTracingPtlasMarkAllDynamicInPartition.Get();
	state.PtlasModeChangeDistance = CVarRayTracingPtlasModeChangeDistance.Get();
#if SPARKLE_WITH_EXTERNAL_CAPTURE
	state.StartupCaptureProvider = CVarExternalCaptureStartupProvider.Get();
	state.CaptureToolsInstalled = HasInstalledExternalCaptureProvider();
#endif
	return state;
}

void EngineRenderingSettingsControls::Apply(const EngineRenderingSettingsState& state) noexcept
{
	const auto setCVarIfChanged = []<typename TCVar, typename TValue>(TCVar& cvar, const TValue& value) noexcept
	{
		if (cvar.Get() != value)
		{
			cvar.Set(value);
		}
	};

	setCVarIfChanged(CVarVSync, state.VSync);
	setCVarIfChanged(CVarVignette, state.Vignette);
	setCVarIfChanged(CVarChromaticAberration, state.ChromaticAberration);
	setCVarIfChanged(CVarBackBufferFormat, state.BackBufferFormat);
	setCVarIfChanged(CVarPreferHighPerformanceAdapter, state.PreferHighPerformanceAdapter);
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
	setCVarIfChanged(CVarRendererMeshAutoBatching, state.MeshAutoBatching);
	setCVarIfChanged(CVarRayTracingTlasRefit, state.RefitTlas);
	setCVarIfChanged(CVarRayTracingPreferPtlas, state.PtlasActive);
	setCVarIfChanged(CVarRayTracingPartitionsPerAxis, state.PtlasPartitionsPerAxis);
	setCVarIfChanged(CVarRayTracingPtlasPartitionUpdateMode, state.PtlasPartitionUpdateMode);
	setCVarIfChanged(CVarRayTracingPtlasMarkAllDynamicInPartition, state.PtlasMarkAllDynamicInPartition);
	setCVarIfChanged(CVarRayTracingPtlasModeChangeDistance, state.PtlasModeChangeDistance);
#if SPARKLE_WITH_EXTERNAL_CAPTURE
	setCVarIfChanged(CVarExternalCaptureStartupProvider, state.StartupCaptureProvider);
#endif
}
