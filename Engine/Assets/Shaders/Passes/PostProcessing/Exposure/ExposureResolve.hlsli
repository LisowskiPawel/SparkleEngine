#pragma once

#include "/Engine/Display/Exposure.hlsli"
#include "/Engine/Resources/FrameUniformData.hlsli"

float ResolveExposureTarget(float averageLuminance, float sampleCount, bool bHistoryValid, float previousExposure)
{
	float targetExposure = Exposure::ComputeExposure(ExposureMode, ManualExposure, ExposureCompensation, ExposureTargetLuminance, ExposureMin, ExposureMax, averageLuminance);

	if (ExposureMode == Exposure::ExposureModeAutomatic && sampleCount <= 0.0f)
	{
		// An invalid HDR frame cannot drive adaptation; the first frame uses neutral exposure.
		targetExposure = clamp(bHistoryValid && isfinite(previousExposure) && previousExposure > 0.0f ? previousExposure : 1.0f, ExposureMin, ExposureMax);
	}

	return targetExposure;
}

float4 ResolveExposurePayload(float averageLuminance, float sampleCount, float4 previousPayload)
{
	const bool bHistoryValid = ExposureHistoryValid != 0u && previousPayload.a == float(ExposureMode);

	const float targetExposure = ResolveExposureTarget(averageLuminance, sampleCount, bHistoryValid, previousPayload.r);

	const float exposure = clamp(Exposure::AdaptExposure(ExposureMode, bHistoryValid, previousPayload.r, targetExposure, DeltaTimeSeconds, ExposureAdaptationSpeedUp, ExposureAdaptationSpeedDown),
	                             ExposureMin,
	                             ExposureMax);

	return float4(exposure, averageLuminance, targetExposure, float(ExposureMode));
}
