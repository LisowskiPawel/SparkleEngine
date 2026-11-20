#pragma once

#include "/Engine/Resources/LightGpuData.hlsli"
#include "/Engine/Resources/SceneLightingUniformData.hlsli"

#include "/Engine/Lighting/AreaLights.hlsli"
#include "/Engine/Lighting/SurfaceLighting.hlsli"
#include "/Engine/RayTracing/RayTracingHitSurface.hlsli"
#include "/Engine/RayTracing/Shadows/RayTracedShadowVisibility.hlsli"

void AccumulateRayTracingHitDirectLightSample(RaytracingAccelerationStructure sceneTlas,
                                              RayTracingHitSurfaceData surface,
                                              float3 viewDirWorld,
                                              LightSampling::DirectLightSample lightSample,
                                              bool bCastsShadow,
                                              bool bTraceShadows,
                                              inout float3 incidentRadiance)
{
	if (!lightSample.bValid)
	{
		return;
	}

	float visibility = 1.0f;
	if (bTraceShadows)
	{
		visibility = TraceDirectLightSample(sceneTlas, surface.PositionWorld, surface.NormalWorld, lightSample, bCastsShadow).Visibility;
	}

	SurfaceLighting::Surface lightingSurface =
	    {viewDirWorld, surface.NormalWorld, surface.BaseColor, surface.Roughness, surface.Metallic, surface.DielectricF0, surface.SubsurfaceColor, surface.SubsurfaceStrength, true, true, true};

	const SurfaceLighting::Contribution contribution = SurfaceLighting::EvaluateDirectLightSample(lightingSurface, lightSample, visibility);

	incidentRadiance += contribution.Diffuse + contribution.Specular + contribution.Subsurface;
}

void AccumulateRayTracingHitDirectionalLights(RaytracingAccelerationStructure sceneTlas,
                                              RayTracingHitSurfaceData surface,
                                              float3 viewDirWorld,
                                              uint pathSampleIndex,
                                              uint bounceIndex,
                                              uint randomFrameIndex,
                                              bool bTraceShadows,
                                              inout float3 incidentRadiance)
{
	const uint dimensionTag = 10u + bounceIndex * 4u;
	const uint sampleFrameIndex = randomFrameIndex + pathSampleIndex * 4099u;
	[loop]
	for (uint lightIndex = 0u; lightIndex < SceneLighting.DirectionalLightCount; ++lightIndex)
	{
		const float2 shapeSample = LightSampling::StableLightSample2D(surface.PositionWorld, lightIndex, dimensionTag, sampleFrameIndex);
		const LightSampling::DirectLightSample lightSample = AreaLights::SampleDirectionalLight(lightIndex, shapeSample);
		const bool bCastsShadow = DirectionalLights[lightIndex].CastShadow != 0u;

		AccumulateRayTracingHitDirectLightSample(sceneTlas, surface, viewDirWorld, lightSample, bCastsShadow, bTraceShadows, incidentRadiance);
	}
}

void AccumulateRayTracingHitPointLights(RaytracingAccelerationStructure sceneTlas,
                                        RayTracingHitSurfaceData surface,
                                        float3 viewDirWorld,
                                        uint pathSampleIndex,
                                        uint bounceIndex,
                                        uint randomFrameIndex,
                                        bool bTraceShadows,
                                        inout float3 incidentRadiance)
{
	const uint dimensionTag = 11u + bounceIndex * 4u;
	const uint sampleFrameIndex = randomFrameIndex + pathSampleIndex * 4099u;
	[loop]
	for (uint lightIndex = 0u; lightIndex < SceneLighting.PointLightCount; ++lightIndex)
	{
		const float2 shapeSample = LightSampling::StableLightSample2D(surface.PositionWorld, lightIndex, dimensionTag, sampleFrameIndex);
		const LightSampling::DirectLightSample lightSample = AreaLights::SamplePointLight(surface.PositionWorld, lightIndex, shapeSample);
		const bool bCastsShadow = PointLights[lightIndex].CastShadow != 0u;

		AccumulateRayTracingHitDirectLightSample(sceneTlas, surface, viewDirWorld, lightSample, bCastsShadow, bTraceShadows, incidentRadiance);
	}
}

void AccumulateRayTracingHitSpotLights(RaytracingAccelerationStructure sceneTlas,
                                       RayTracingHitSurfaceData surface,
                                       float3 viewDirWorld,
                                       uint pathSampleIndex,
                                       uint bounceIndex,
                                       uint randomFrameIndex,
                                       bool bTraceShadows,
                                       inout float3 incidentRadiance)
{
	const uint dimensionTag = 12u + bounceIndex * 4u;
	const uint sampleFrameIndex = randomFrameIndex + pathSampleIndex * 4099u;
	[loop]
	for (uint lightIndex = 0u; lightIndex < SceneLighting.SpotLightCount; ++lightIndex)
	{
		const float2 shapeSample = LightSampling::StableLightSample2D(surface.PositionWorld, lightIndex, dimensionTag, sampleFrameIndex);
		const LightSampling::DirectLightSample lightSample = AreaLights::SampleSpotLight(surface.PositionWorld, lightIndex, shapeSample);
		const bool bCastsShadow = SpotLights[lightIndex].CastShadow != 0u;

		AccumulateRayTracingHitDirectLightSample(sceneTlas, surface, viewDirWorld, lightSample, bCastsShadow, bTraceShadows, incidentRadiance);
	}
}

void AccumulateRayTracingHitRectLights(RaytracingAccelerationStructure sceneTlas,
                                       RayTracingHitSurfaceData surface,
                                       float3 viewDirWorld,
                                       uint pathSampleIndex,
                                       uint bounceIndex,
                                       uint randomFrameIndex,
                                       bool bTraceShadows,
                                       inout float3 incidentRadiance)
{
	const uint dimensionTag = 13u + bounceIndex * 4u;
	const uint sampleFrameIndex = randomFrameIndex + pathSampleIndex * 4099u;
	[loop]
	for (uint lightIndex = 0u; lightIndex < SceneLighting.RectLightCount; ++lightIndex)
	{
		const float2 shapeSample = LightSampling::StableLightSample2D(surface.PositionWorld, lightIndex, dimensionTag, sampleFrameIndex);
		const LightSampling::DirectLightSample lightSample = AreaLights::SampleRectLight(surface.PositionWorld, lightIndex, shapeSample);
		const bool bCastsShadow = RectLights[lightIndex].CastShadow != 0u;

		AccumulateRayTracingHitDirectLightSample(sceneTlas, surface, viewDirWorld, lightSample, bCastsShadow, bTraceShadows, incidentRadiance);
	}
}
