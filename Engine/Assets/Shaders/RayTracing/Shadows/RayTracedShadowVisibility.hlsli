#ifndef SPARKLE_RAY_TRACED_SHADOW_VISIBILITY_HLSLI
#define SPARKLE_RAY_TRACED_SHADOW_VISIBILITY_HLSLI

#include "/Engine/Lighting/LightSampling.hlsli"
#include "/Engine/RayTracing/Shadows/RayTracedShadowSignals.hlsli"

#include "/Engine/RayTracing/Shadows/RayTracedShadowTrace.hlsli"

ShadowVisibilitySample TraceDirectLightSample(RaytracingAccelerationStructure sceneTlas, float3 positionWorld, float3 normalWorld, LightSampling::DirectLightSample lightSample, bool bCastsShadow)
{
	RayTracedShadowRequest request;
	ShadowVisibilitySample signal;
	if (ShadowRays::PrepareDirectLightShadow(positionWorld, normalWorld, lightSample, bCastsShadow, request, signal))
	{
		return ShadowRays::TraceShadowRay(sceneTlas, request);
	}

	return signal;
}

#endif
