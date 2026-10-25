#pragma once

#include "/Engine/Resources/LightGpuData.hlsli"
#include "/Engine/Resources/SceneLightingUniformData.hlsli"

#include "/Engine/Lighting/AreaLights.hlsli"
#include "/Engine/Lighting/SurfaceLighting.hlsli"
#include "/Engine/RayTracing/RayTracingMaterialHit.hlsli"
#include "/Engine/RayTracing/Shadows/RayTracedShadowVisibility.hlsli"

void AccumulateRayTracingHitDirectLightSample(RayTracingHitSurfaceData surface,
                                              float3 viewDirWorld,
                                              LightSampling::DirectLightSample lightSample,
                                              bool castsShadow,
                                              bool traceShadows,
                                              inout float3 incidentRadiance)
{
	if (!lightSample.Valid)
	{
		return;
	}

	float visibility = 1.0f;
	if (traceShadows)
	{
		visibility = TraceDirectLightSample(surface.PositionWorld, surface.NormalWorld, lightSample, castsShadow).Visibility;
	}

	SurfaceLighting::Surface lightingSurface =
	    {viewDirWorld, surface.NormalWorld, surface.BaseColor, surface.Roughness, surface.Metallic, surface.DielectricF0, surface.SubsurfaceColor, surface.SubsurfaceStrength, true, true, true};

	const SurfaceLighting::Contribution contribution = SurfaceLighting::EvaluateDirectLightSample(lightingSurface, lightSample, visibility);

	incidentRadiance += contribution.Diffuse + contribution.Specular + contribution.Subsurface;
}

void AccumulateRayTracingHitDirectionalLights(RayTracingHitSurfaceData surface,
                                              float3 viewDirWorld,
                                              uint pathSampleIndex,
                                              uint bounceIndex,
                                              uint randomFrameIndex,
                                              bool traceShadows,
                                              inout float3 incidentRadiance)
{
	[loop]
	for (uint lightIndex = 0u; lightIndex < SceneLighting.DirectionalLightCount; ++lightIndex)
	{
		const float2 shapeSample = LightSampling::StableLightSample2D(surface.PositionWorld, lightIndex, 10u + bounceIndex * 4u, randomFrameIndex + pathSampleIndex * 4099u);
		const LightSampling::DirectLightSample lightSample = AreaLights::SampleDirectionalLight(lightIndex, shapeSample);

		AccumulateRayTracingHitDirectLightSample(surface, viewDirWorld, lightSample, DirectionalLights[lightIndex].CastShadow != 0u, traceShadows, incidentRadiance);
	}
}

void AccumulateRayTracingHitPointLights(RayTracingHitSurfaceData surface,
                                        float3 viewDirWorld,
                                        uint pathSampleIndex,
                                        uint bounceIndex,
                                        uint randomFrameIndex,
                                        bool traceShadows,
                                        inout float3 incidentRadiance)
{
	[loop]
	for (uint lightIndex = 0u; lightIndex < SceneLighting.PointLightCount; ++lightIndex)
	{
		const float2 shapeSample = LightSampling::StableLightSample2D(surface.PositionWorld, lightIndex, 11u + bounceIndex * 4u, randomFrameIndex + pathSampleIndex * 4099u);
		const LightSampling::DirectLightSample lightSample = AreaLights::SamplePointLight(surface.PositionWorld, lightIndex, shapeSample);

		AccumulateRayTracingHitDirectLightSample(surface, viewDirWorld, lightSample, PointLights[lightIndex].CastShadow != 0u, traceShadows, incidentRadiance);
	}
}

void AccumulateRayTracingHitSpotLights(RayTracingHitSurfaceData surface,
                                       float3 viewDirWorld,
                                       uint pathSampleIndex,
                                       uint bounceIndex,
                                       uint randomFrameIndex,
                                       bool traceShadows,
                                       inout float3 incidentRadiance)
{
	[loop]
	for (uint lightIndex = 0u; lightIndex < SceneLighting.SpotLightCount; ++lightIndex)
	{
		const float2 shapeSample = LightSampling::StableLightSample2D(surface.PositionWorld, lightIndex, 12u + bounceIndex * 4u, randomFrameIndex + pathSampleIndex * 4099u);
		const LightSampling::DirectLightSample lightSample = AreaLights::SampleSpotLight(surface.PositionWorld, lightIndex, shapeSample);

		AccumulateRayTracingHitDirectLightSample(surface, viewDirWorld, lightSample, SpotLights[lightIndex].CastShadow != 0u, traceShadows, incidentRadiance);
	}
}

void AccumulateRayTracingHitRectLights(RayTracingHitSurfaceData surface,
                                       float3 viewDirWorld,
                                       uint pathSampleIndex,
                                       uint bounceIndex,
                                       uint randomFrameIndex,
                                       bool traceShadows,
                                       inout float3 incidentRadiance)
{
	[loop]
	for (uint lightIndex = 0u; lightIndex < SceneLighting.RectLightCount; ++lightIndex)
	{
		const float2 shapeSample = LightSampling::StableLightSample2D(surface.PositionWorld, lightIndex, 13u + bounceIndex * 4u, randomFrameIndex + pathSampleIndex * 4099u);
		const LightSampling::DirectLightSample lightSample = AreaLights::SampleRectLight(surface.PositionWorld, lightIndex, shapeSample);

		AccumulateRayTracingHitDirectLightSample(surface, viewDirWorld, lightSample, RectLights[lightIndex].CastShadow != 0u, traceShadows, incidentRadiance);
	}
}
