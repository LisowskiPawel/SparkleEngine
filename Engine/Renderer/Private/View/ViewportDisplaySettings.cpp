#include "PCH.h"

#include "View/ViewportDisplaySettings.h"

#include "View/ViewportDisplayCVars.h"
#include "Renderer/Public/Settings/EngineRenderingSettings.h"

#include <algorithm>
#include <cmath>

namespace ViewportDisplaySettings
{
	EngineExposureMode ResolveMode(EngineExposureMode requested, EngineExposureMode fallback) noexcept
	{
		switch (requested)
		{
			case EngineExposureMode::Manual:
			case EngineExposureMode::Automatic:
				return requested;
			default:
				return fallback;
		}
	}

	EngineExposureMeteringMethod ResolveMeteringMethod(EngineExposureMeteringMethod requested, EngineExposureMeteringMethod fallback) noexcept
	{
		switch (requested)
		{
			case EngineExposureMeteringMethod::Histogram:
			case EngineExposureMeteringMethod::DownsamplePyramid:
				return requested;
			default:
				return fallback;
		}
	}

	ResolvedViewportDisplaySettings ReadGlobalSettings() noexcept
	{
		return ResolvedViewportDisplaySettings{
		    .ToneMapper = CVarToneMapper.Get(),
		    .ExposureMode = CVarExposureMode.Get(),
		    .ExposureMeteringMethod = CVarExposureMeteringMethod.Get(),
		    .ManualExposure = CVarManualExposure.Get(),
		    .ExposureCompensation = CVarExposureCompensation.Get(),
		    .ExposureTargetLuminance = CVarExposureTargetLuminance.Get(),
		    .ExposureMin = CVarExposureMin.Get(),
		    .ExposureMax = CVarExposureMax.Get(),
		    .ExposureAdaptationSpeedUp = CVarExposureAdaptationSpeedUp.Get(),
		    .ExposureAdaptationSpeedDown = CVarExposureAdaptationSpeedDown.Get()};
	}

	void ApplyExposureOverrides(ResolvedViewportDisplaySettings& resolved, const ViewportExposureOverrides& overrides) noexcept
	{
		resolved.ExposureMode = overrides.OverrideMode ? ResolveMode(overrides.Mode, resolved.ExposureMode) : resolved.ExposureMode;
		resolved.ExposureMeteringMethod = overrides.OverrideMeteringMethod ? ResolveMeteringMethod(overrides.MeteringMethod, resolved.ExposureMeteringMethod) : resolved.ExposureMeteringMethod;
		resolved.ManualExposure = overrides.OverrideManualExposure ? overrides.ManualExposure : resolved.ManualExposure;
		resolved.ExposureCompensation = overrides.OverrideCompensation ? overrides.Compensation : resolved.ExposureCompensation;
		resolved.ExposureTargetLuminance = overrides.OverrideTargetLuminance ? overrides.TargetLuminance : resolved.ExposureTargetLuminance;
		resolved.ExposureMin = overrides.OverrideMinimum ? overrides.Minimum : resolved.ExposureMin;
		resolved.ExposureMax = overrides.OverrideMaximum ? overrides.Maximum : resolved.ExposureMax;
		resolved.ExposureAdaptationSpeedUp = overrides.OverrideAdaptationSpeedUp ? overrides.AdaptationSpeedUp : resolved.ExposureAdaptationSpeedUp;
		resolved.ExposureAdaptationSpeedDown = overrides.OverrideAdaptationSpeedDown ? overrides.AdaptationSpeedDown : resolved.ExposureAdaptationSpeedDown;
	}

	void ValidateDisplayModes(const ResolvedViewportDisplaySettings& resolved) noexcept
	{
		switch (resolved.ExposureMode)
		{
			case EngineExposureMode::Manual:
			case EngineExposureMode::Automatic:
				break;
			default:
			{
				SPARKLE_DEFINE_LOG_CATEGORY_STATIC(LogExposureSettings, "Renderer.ExposureSettings");
				Diagnostics::Fatal(LogExposureSettings, __FILE__, __LINE__, "Exposure settings contain an unknown exposure mode.");
			}
		}
		switch (resolved.ToneMapper)
		{
			case EngineToneMapper::Reinhard:
			case EngineToneMapper::AcesApprox:
			case EngineToneMapper::AcesFilmic:
				break;
			default:
			{
				SPARKLE_DEFINE_LOG_CATEGORY_STATIC(LogToneMappingSettings, "Renderer.ToneMappingSettings");
				Diagnostics::Fatal(LogToneMappingSettings, __FILE__, __LINE__, "Tone-mapping settings contain an unknown tone mapper.");
			}
		}
	}

	void SanitizeExposure(ResolvedViewportDisplaySettings& resolved) noexcept
	{
		constexpr EngineRenderingSettingsState defaults;
		constexpr float maximumExposureMultiplier = 65536.0f;

		const auto finiteOrDefault = [](float value, float fallback)
		{
			return std::isfinite(value) ? value : fallback;
		};

		resolved.ManualExposure = finiteOrDefault(resolved.ManualExposure, defaults.ManualExposure);
		resolved.ExposureCompensation = finiteOrDefault(resolved.ExposureCompensation, defaults.ExposureCompensation);
		resolved.ExposureTargetLuminance = finiteOrDefault(resolved.ExposureTargetLuminance, defaults.ExposureTargetLuminance);
		resolved.ExposureMin = finiteOrDefault(resolved.ExposureMin, defaults.ExposureMin);
		resolved.ExposureMax = finiteOrDefault(resolved.ExposureMax, defaults.ExposureMax);
		resolved.ExposureAdaptationSpeedUp = finiteOrDefault(resolved.ExposureAdaptationSpeedUp, defaults.ExposureAdaptationSpeedUp);
		resolved.ExposureAdaptationSpeedDown = finiteOrDefault(resolved.ExposureAdaptationSpeedDown, defaults.ExposureAdaptationSpeedDown);

		resolved.ManualExposure = (std::max) (resolved.ManualExposure, 0.0f);
		resolved.ExposureCompensation = std::clamp(resolved.ExposureCompensation, -16.0f, 16.0f);
		resolved.ExposureTargetLuminance = (std::max) (resolved.ExposureTargetLuminance, 0.0001f);
		resolved.ExposureMin = std::clamp(resolved.ExposureMin, 0.0f, maximumExposureMultiplier);
		resolved.ExposureMax = std::clamp(resolved.ExposureMax, resolved.ExposureMin, maximumExposureMultiplier);
		resolved.ExposureAdaptationSpeedUp = (std::max) (resolved.ExposureAdaptationSpeedUp, 0.0f);
		resolved.ExposureAdaptationSpeedDown = (std::max) (resolved.ExposureAdaptationSpeedDown, 0.0f);
	}
}

ResolvedViewportDisplaySettings ResolvedViewportDisplaySettings::Resolve(const ViewportExposureOverrides& overrides) noexcept
{
	auto resolved = ViewportDisplaySettings::ReadGlobalSettings();
	ViewportDisplaySettings::ApplyExposureOverrides(resolved, overrides);
	ViewportDisplaySettings::ValidateDisplayModes(resolved);
	ViewportDisplaySettings::SanitizeExposure(resolved);
	return resolved;
}
