#ifndef SPARKLE_RAY_TRACED_SHADOW_SEMANTICS_HLSLI
#define SPARKLE_RAY_TRACED_SHADOW_SEMANTICS_HLSLI

#include "/Engine/Lighting/LightSampling.hlsli"
#include "/Engine/RayTracing/Shadows/RayTracedShadowSignals.hlsli"

struct RayTracedShadowRequest
{
	float3 OriginWorld;
	float3 DirectionWorld;
	float MaxDistance;
};

namespace ShadowRays
{
	static const uint ShadowInstanceMask = 0xFFu;
	static const uint ShadowRayFlags = RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES;
	static const float MinimumShadowTMin = 0.001f;

	bool PrepareDirectLightShadow(float3 positionWorld,
	                              float3 normalWorld,
	                              LightSampling::DirectLightSample lightSample,
	                              bool bCastsShadow,
	                              out RayTracedShadowRequest request,
	                              out ShadowVisibilitySample immediateSignal)
	{
		request = (RayTracedShadowRequest)0;
		immediateSignal = BuildUnshadowedSignal(0.0f);
		if (!lightSample.bValid)
		{
			return false;
		}

		const bool bSupportedIsDirectional = lightSample.bIsDirectional;
		const bool bSupportedShadowsEnabled = bSupportedIsDirectional && (RayTracedDirectionalShadowsEnabled != 0u);
		const bool bSupportedShadowsEnabled2 = !bSupportedIsDirectional && (RayTracedLocalLightShadowsEnabled != 0u);
		const bool bSupported = bSupportedShadowsEnabled || bSupportedShadowsEnabled2;
		const float maxDistance = lightSample.bIsDirectional ? RayTracedShadowMaxDistance : lightSample.VisibilityDistance;
		immediateSignal = BuildUnshadowedSignal(maxDistance);
		if (!bCastsShadow || !bSupported || (!lightSample.bIsDirectional && lightSample.VisibilityDistance <= MinimumShadowTMin))
		{
			return false;
		}

		request.OriginWorld = positionWorld + normalize(normalWorld) * RayTracedShadowNormalBias;
		request.DirectionWorld = lightSample.DirectionWorld;
		request.MaxDistance = lightSample.bIsDirectional ? RayTracedShadowMaxDistance : max(lightSample.VisibilityDistance - MinimumShadowTMin, MinimumShadowTMin);
		return true;
	}

	ShadowVisibilitySample ResolveTrace(bool bOccluded, float hitDistance, float maxDistance)
	{
		if (bOccluded)
		{
			return BuildOccludedSignal(hitDistance, maxDistance);
		}

		return BuildUnshadowedSignal(maxDistance);
	}
}

#endif
