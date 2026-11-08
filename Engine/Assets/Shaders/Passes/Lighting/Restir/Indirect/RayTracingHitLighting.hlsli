#pragma once

#include "/Engine/Passes/Lighting/Restir/Indirect/RayTracingHitDirectLighting.hlsli"

float3 ShadeRayTracingHitIncidentRadiance(RaytracingAccelerationStructure sceneTlas,
                                          RayTracingHitSurfaceData surface,
                                          float3 rayDirectionWorld,
                                          uint pathSampleIndex,
                                          uint bounceIndex,
                                          uint randomFrameIndex,
                                          bool bTraceShadows)
{
	if (!surface.bValid)
	{
		return 0.0f.xxx;
	}

	float3 incidentRadiance = max(surface.EmissiveColor, 0.0f.xxx);
	const float3 viewDirWorld = normalize(-rayDirectionWorld);

	AccumulateRayTracingHitDirectionalLights(sceneTlas, surface, viewDirWorld, pathSampleIndex, bounceIndex, randomFrameIndex, bTraceShadows, incidentRadiance);
	AccumulateRayTracingHitPointLights(sceneTlas, surface, viewDirWorld, pathSampleIndex, bounceIndex, randomFrameIndex, bTraceShadows, incidentRadiance);
	AccumulateRayTracingHitSpotLights(sceneTlas, surface, viewDirWorld, pathSampleIndex, bounceIndex, randomFrameIndex, bTraceShadows, incidentRadiance);
	AccumulateRayTracingHitRectLights(sceneTlas, surface, viewDirWorld, pathSampleIndex, bounceIndex, randomFrameIndex, bTraceShadows, incidentRadiance);

	return max(incidentRadiance, 0.0f.xxx);
}
