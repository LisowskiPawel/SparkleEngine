#ifndef SPARKLE_DISPLAY_EXPOSURE_HLSLI
#define SPARKLE_DISPLAY_EXPOSURE_HLSLI

#include "/Engine/Common/Color.hlsli"

namespace Exposure
{
	static const uint ExposureModeManual = 0u;
	static const uint ExposureModeAutomatic = 1u;

	static const float MinimumMeteredLuminance = 1.0e-4f;

	float2 BuildLogLuminanceMoment(float3 linearHdrColor)
	{
		if (!all(isfinite(linearHdrColor)))
		{
			return 0.0f.xx;
		}

		const float3 positiveColor = CommonColor::ClampPositive(linearHdrColor);
		const float sceneLuminance = CommonColor::LuminanceRec709(positiveColor);
		const float luminance = max(sceneLuminance, MinimumMeteredLuminance);
		return float2(log(luminance), 1.0f);
	}

	float ResolveAverageLuminance(float2 moments)
	{
		const float sampleCount = max(moments.y, 1.0f);
		return exp(moments.x / sampleCount);
	}

	float ComputeExposure(uint exposureMode, float manualExposure, float exposureCompensation, float targetLuminance, float minExposure, float maxExposure, float sceneAverageLuminance)
	{
		const float compensationEv = clamp(exposureCompensation, -16.0f, 16.0f);
		const float compensation = exp2(compensationEv);
		const float safeTargetLuminance = max(targetLuminance, MinimumMeteredLuminance);
		const float safeAverageLuminance = max(sceneAverageLuminance, MinimumMeteredLuminance);

		const float unclampedExposure = exposureMode == ExposureModeManual ? max(manualExposure, 0.0f) * compensation : (safeTargetLuminance / safeAverageLuminance) * compensation;

		const float safeMinExposure = max(minExposure, 0.0f);
		const float safeMaxExposure = max(maxExposure, safeMinExposure);
		return clamp(unclampedExposure, safeMinExposure, safeMaxExposure);
	}

	float AdaptExposure(uint exposureMode, bool bExposureHistoryValid, float previousExposure, float targetExposure, float frameDeltaSeconds, float adaptationSpeedUp, float adaptationSpeedDown)
	{
		if (exposureMode == ExposureModeManual || !bExposureHistoryValid)
		{
			return targetExposure;
		}

		if (!isfinite(previousExposure) || previousExposure <= 0.0f)
		{
			return targetExposure;
		}

		const float safePreviousExposure = previousExposure;
		const float safeTargetExposure = max(targetExposure, 1.0e-20f);
		const float selectedSpeed = safeTargetExposure > safePreviousExposure ? adaptationSpeedUp : adaptationSpeedDown;
		const float adaptationSpeed = max(selectedSpeed, 0.0f);
		const float elapsedSeconds = max(frameDeltaSeconds, 0.0f);
		const float decay = exp(-elapsedSeconds * adaptationSpeed);
		const float adaptationT = saturate(1.0f - decay);
		const float previousExposureEv = log2(safePreviousExposure);
		const float targetExposureEv = log2(safeTargetExposure);
		const float adaptedExposureEv = lerp(previousExposureEv, targetExposureEv, adaptationT);
		return exp2(adaptedExposureEv);
	}
}

#endif
