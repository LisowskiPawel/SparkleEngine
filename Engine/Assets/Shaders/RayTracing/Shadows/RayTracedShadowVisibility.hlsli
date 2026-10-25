#ifndef SPARKLE_RAY_TRACED_SHADOW_VISIBILITY_HLSLI
#define SPARKLE_RAY_TRACED_SHADOW_VISIBILITY_HLSLI

#include "/Engine/Lighting/LightSampling.hlsli"
#include "/Engine/RayTracing/Shadows/RayTracedShadowSignals.hlsli"

#include "/Engine/RayTracing/Shadows/RayTracedShadowTrace.hlsli"

ShadowVisibilitySample TraceDirectLightSample(float3 positionWorld, float3 normalWorld, LightSampling::DirectLightSample lightSample, bool castsShadow)
{
	RayTracedShadowRequest request;
	ShadowVisibilitySample signal;
	if (ShadowRays::PrepareDirectLightShadow(positionWorld, normalWorld, lightSample, castsShadow, request, signal))
	{
		return ShadowRays::TraceShadowRay(request);
	}

	return signal;
}

#endif
