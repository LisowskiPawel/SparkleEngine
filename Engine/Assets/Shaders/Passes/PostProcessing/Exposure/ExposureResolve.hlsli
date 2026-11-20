#pragma once

#include "/Engine/Display/Exposure.hlsli"
#include "/Engine/Resources/FrameUniformData.hlsli"

float ResolveExposureTarget(float averageLuminance, float sampleCount, bool bHistoryValid, float previousExposure)
{
	float targetExposure = Exposure::ComputeExposure(ExposureMode, ManualExposure, ExposureCompensation, ExposureTargetLuminance, ExposureMin, ExposureMax, averageLuminance);

	if (ExposureMode == Exposure::ExposureModeAutomatic && sampleCount <= 0.0f)
	{
		// An invalid HDR frame cannot drive adaptation; the first frame uses neutral exposure.
		const bool bPreviousExposureFinite = bHistoryValid && isfinite(previousExposure);
		const bool bPreviousExposureUsable = bPreviousExposureFinite && previousExposure > 0.0f;
		const float fallbackExposure = bPreviousExposureUsable ? previousExposure : 1.0f;
		targetExposure = clamp(fallbackExposure, ExposureMin, ExposureMax);
	}

	return targetExposure;
}

float4 ResolveExposurePayload(float averageLuminance, float sampleCount, float4 previousPayload)
{
	const bool bHistoryValid = ExposureHistoryValid != 0u && previousPayload.a == float(ExposureMode);

	const float targetExposure = ResolveExposureTarget(averageLuminance, sampleCount, bHistoryValid, previousPayload.r);

	const float adaptedExposure = Exposure::AdaptExposure(ExposureMode, bHistoryValid, previousPayload.r, targetExposure, DeltaTimeSeconds, ExposureAdaptationSpeedUp, ExposureAdaptationSpeedDown);

	const float exposure = clamp(adaptedExposure, ExposureMin, ExposureMax);

	return float4(exposure, averageLuminance, targetExposure, float(ExposureMode));
}
