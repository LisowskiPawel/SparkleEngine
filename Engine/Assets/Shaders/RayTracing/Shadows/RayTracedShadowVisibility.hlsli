#ifndef SPARKLE_RAY_TRACED_SHADOW_VISIBILITY_HLSLI
#define SPARKLE_RAY_TRACED_SHADOW_VISIBILITY_HLSLI

#include "/Engine/Lighting/LightSampling.hlsli"
#include "/Engine/RayTracing/Shadows/RayTracedShadowSignals.hlsli"

#include "/Engine/RayTracing/Shadows/RayTracedShadowTrace.hlsli"

namespace RayTracedShadowVisibility
{
	ShadowVisibilitySample TraceDirectLightSample(float3 positionWorld, float3 normalWorld, LightSampling::DirectLightSample lightSample, bool castsShadow)
	{
		const RayTracedShadowPreparation preparation = RayTracedShadows::PrepareDirectLightShadow(positionWorld, normalWorld, lightSample, castsShadow);
		if (preparation.TraceRequired)
		{
			return RayTracedShadows::TraceShadowRay(preparation.Request);
		}

		return preparation.ImmediateSignal;
	}
}

#endif
