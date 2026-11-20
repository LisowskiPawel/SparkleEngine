#ifndef SPARKLE_PUNCTUAL_LIGHTS_HLSLI
#define SPARKLE_PUNCTUAL_LIGHTS_HLSLI

namespace PunctualLights
{

	float3 DirectionToLight(float3 positionWorld, float3 lightPositionWorld, out float distanceToLight)
	{
		const float3 surfaceToLight = lightPositionWorld - positionWorld;
		distanceToLight = length(surfaceToLight);
		return surfaceToLight / max(distanceToLight, 0.0001f);
	}

	float ComputeRangeAttenuation(float distanceToLight, float range)
	{
		if (range <= 0.0f)
		{
			return 1.0f;
		}

		const float normalizedDistance = distanceToLight / max(range, 1.0e-4f);
		const float smoothRange = saturate(1.0f - normalizedDistance * normalizedDistance * normalizedDistance * normalizedDistance);
		return smoothRange * smoothRange;
	}

	float ComputeDistanceAttenuationDenominator(float distanceToLight, float3 distanceAttenuationCoefficients)
	{
		const float linearTerm = distanceAttenuationCoefficients.y * distanceToLight;
		const float quadraticTerm = distanceAttenuationCoefficients.z * distanceToLight * distanceToLight;
		return distanceAttenuationCoefficients.x + linearTerm + quadraticTerm;
	}

	float ComputePunctualDistanceAttenuation(float distanceToLight, float range, float3 distanceAttenuationCoefficients)
	{
		const float denominator = ComputeDistanceAttenuationDenominator(distanceToLight, distanceAttenuationCoefficients);
		const float rangeAttenuation = ComputeRangeAttenuation(distanceToLight, range);
		return rcp(max(denominator, 1.0e-4f)) * rangeAttenuation;
	}

	float ComputeAreaDistanceAttenuationCorrection(float distanceToLight, float3 distanceAttenuationCoefficients)
	{
		const float distanceSquared = max(distanceToLight * distanceToLight, 1.0e-4f);
		const float denominator = ComputeDistanceAttenuationDenominator(distanceToLight, distanceAttenuationCoefficients);
		return distanceSquared / max(denominator, 1.0e-4f);
	}

	float ComputeSpotAngularAttenuation(float3 lightToSurfaceDirection, float3 spotDirection, float innerAngleCosine, float outerAngleCosine)
	{
		const float coneCosine = dot(normalize(lightToSurfaceDirection), normalize(spotDirection));
		const float angularTransition = max(innerAngleCosine - outerAngleCosine, 0.0001f);
		const float angularAttenuation = saturate((coneCosine - outerAngleCosine) / angularTransition);
		return angularAttenuation * angularAttenuation;
	}
}

#endif
