#pragma once

#include "/Engine/RayTracing/RayTracingHitDirectLighting.hlsli"

float3 ShadeRayTracingHitIncidentRadiance(RayTracingHitSurfaceData surface, float3 rayDirectionWorld, uint pathSampleIndex, uint bounceIndex, uint randomFrameIndex, bool traceShadows)
{
	if (!surface.Valid)
	{
		return 0.0f.xxx;
	}

	float3 incidentRadiance = max(surface.EmissiveColor, 0.0f.xxx);
	const float3 viewDirWorld = normalize(-rayDirectionWorld);

	AccumulateRayTracingHitDirectionalLights(surface, viewDirWorld, pathSampleIndex, bounceIndex, randomFrameIndex, traceShadows, incidentRadiance);
	AccumulateRayTracingHitPointLights(surface, viewDirWorld, pathSampleIndex, bounceIndex, randomFrameIndex, traceShadows, incidentRadiance);
	AccumulateRayTracingHitSpotLights(surface, viewDirWorld, pathSampleIndex, bounceIndex, randomFrameIndex, traceShadows, incidentRadiance);
	AccumulateRayTracingHitRectLights(surface, viewDirWorld, pathSampleIndex, bounceIndex, randomFrameIndex, traceShadows, incidentRadiance);

	return max(incidentRadiance, 0.0f.xxx);
}
