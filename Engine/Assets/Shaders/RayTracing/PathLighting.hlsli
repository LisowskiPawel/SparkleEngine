#ifndef SPARKLE_RAY_TRACING_PATH_LIGHTING_HLSLI
#define SPARKLE_RAY_TRACING_PATH_LIGHTING_HLSLI

#include "/Engine/Resources/FrameUniformData.hlsli"

#include "/Engine/Lighting/Sky.hlsli"
#include "/Engine/RayTracing/PathTracer.hlsli"
#include "/Engine/RayTracing/PathSampling.hlsli"
#include "/Engine/RayTracing/PathTrace.hlsli"
#include "/Engine/RayTracing/RayTracingMaterialHit.hlsli"
#include "/Engine/RayTracing/RayTracingHitLighting.hlsli"

namespace PathLighting
{
	struct Result
	{
		float3 DiffuseContribution;
		float3 SpecularContribution;
		PathSample::LightingResult FirstLighting;
	};

	PathSample::LightingResult ResolveLighting(RayTracingTraceResult trace,
	                                           float3 directionWorld,
	                                           Texture2D skyTexture,
	                                           SamplerState skySampler,
	                                           uint pathSampleIndex,
	                                           uint bounceIndex,
	                                           uint randomFrameIndex,
	                                           bool traceSecondaryShadows,
	                                           out RayTracingHitSurfaceData outHitSurface)
	{
		outHitSurface = (RayTracingHitSurfaceData)0;

		PathSample::LightingResult result = (PathSample::LightingResult)0;
		result.Hit = trace.Hit;

		if (trace.Hit)
		{
			outHitSurface = ReconstructRayTracingHitSurface(trace);
			result.Hit = outHitSurface.Valid;
			result.HitPositionWorld = outHitSurface.Valid ? outHitSurface.PositionWorld : 0.0f.xxx;

			result.IncidentRadiance = outHitSurface.Valid ? ShadeRayTracingHitIncidentRadiance(outHitSurface, directionWorld, pathSampleIndex, bounceIndex, randomFrameIndex, traceSecondaryShadows)
			                                              : 0.0f.xxx;

			return result;
		}

		result.IncidentRadiance = SampleSkyRadiance(skyTexture, skySampler, directionWorld);
		return result;
	}

	void AdvanceThroughput(inout PathTracer::PathState path,
	                       PathSample::DirectionSample sample,
	                       uint bounceIndex,
	                       bool evaluatePrimaryDiffuse,
	                       bool evaluatePrimarySpecular,
	                       inout float3 diffuseThroughput,
	                       inout float3 specularThroughput)
	{
		PathTracer::ApplyDirectionSample(path, sample);
		if (bounceIndex == 0u)
		{
			diffuseThroughput = sample.DiffuseThroughput;
			specularThroughput = sample.SpecularThroughput;
		}
		else
		{
			const float3 continuationThroughput = sample.DiffuseThroughput + sample.SpecularThroughput;
			if (evaluatePrimaryDiffuse)
			{
				diffuseThroughput *= continuationThroughput;
			}
			if (evaluatePrimarySpecular)
			{
				specularThroughput *= continuationThroughput;
			}
		}
	}

	bool SurvivesRoulette(inout PathTracer::PathState path,
	                      float random,
	                      uint bounceIndex,
	                      bool evaluatePrimaryDiffuse,
	                      bool evaluatePrimarySpecular,
	                      inout float3 diffuseThroughput,
	                      inout float3 specularThroughput)
	{
		const float survivalProbability = PathSampling::RussianRouletteSurvivalProbability(path.Throughput, bounceIndex);
		if (survivalProbability <= 0.0f || random > survivalProbability)
		{
			return false;
		}
		if (survivalProbability < 1.0f)
		{
			PathTracer::ApplySurvivalCompensation(path.Throughput, survivalProbability);
			if (evaluatePrimaryDiffuse)
			{
				PathTracer::ApplySurvivalCompensation(diffuseThroughput, survivalProbability);
			}
			if (evaluatePrimarySpecular)
			{
				PathTracer::ApplySurvivalCompensation(specularThroughput, survivalProbability);
			}
		}

		return true;
	}

	void AccumulateLighting(inout Result result, float3 diffuseThroughput, float3 specularThroughput, float3 incidentRadiance, bool evaluatePrimaryDiffuse, bool evaluatePrimarySpecular)
	{
		if (evaluatePrimaryDiffuse)
		{
			PathTracer::AddRadiance(result.DiffuseContribution, diffuseThroughput, incidentRadiance);
		}
		if (evaluatePrimarySpecular)
		{
			PathTracer::AddRadiance(result.SpecularContribution, specularThroughput, incidentRadiance);
		}
	}

	Result TraceSurfacePath(Texture2D skyTexture,
	                        SamplerState skySampler,
	                        RayTracingPathSurface primarySurface,
	                        uint2 pixelCoord,
	                        uint sampleIndex,
	                        uint specularSampleMode,
	                        uint bounceCount,
	                        uint randomFrameIndex,
	                        bool evaluatePrimaryDiffuse,
	                        bool evaluatePrimarySpecular,
	                        bool traceSecondaryShadows)
	{
		Result result = (Result)0;

		RayTracingPathSurface surface = primarySurface;
		PathTracer::PathState path = (PathTracer::PathState)0;
		path.Throughput = 1.0f.xxx;
		float3 diffuseThroughput = 0.0f.xxx;
		float3 specularThroughput = 0.0f.xxx;
		const uint sanitizedBounceCount = max(bounceCount, 1u);

		[loop]
		for (uint bounceIndex = 0u; bounceIndex < sanitizedBounceCount; ++bounceIndex)
		{
			const PathSampling::RandomSamples randomSamples = PathSampling::GenerateRandomSamples(pixelCoord, bounceIndex, sampleIndex, randomFrameIndex);

			const PathSample::DirectionSample sample = PathSampling::SampleBSDF(surface,
			                                                                    specularSampleMode,
			                                                                    randomSamples,
			                                                                    bounceIndex != 0u || evaluatePrimaryDiffuse,
			                                                                    bounceIndex != 0u || evaluatePrimarySpecular);

			if (!sample.HasSupport)
			{
				break;
			}

			AdvanceThroughput(path, sample, bounceIndex, evaluatePrimaryDiffuse, evaluatePrimarySpecular, diffuseThroughput, specularThroughput);

			if (!SurvivesRoulette(path, randomSamples.Roulette, bounceIndex, evaluatePrimaryDiffuse, evaluatePrimarySpecular, diffuseThroughput, specularThroughput))
			{
				break;
			}

			float3 rayOriginWorld = 0.0f.xxx;
			const RayTracingTraceResult trace = TraceSurfaceRay(surface, path.DirectionWorld, rayOriginWorld);
			path.OriginWorld = rayOriginWorld;
			RayTracingHitSurfaceData hitSurface;

			const PathSample::LightingResult
			    lighting = ResolveLighting(trace, sample.DirectionWorld, skyTexture, skySampler, sampleIndex, bounceIndex, randomFrameIndex, traceSecondaryShadows, hitSurface);

			AccumulateLighting(result, diffuseThroughput, specularThroughput, lighting.IncidentRadiance, evaluatePrimaryDiffuse, evaluatePrimarySpecular);

			if (bounceIndex == 0u)
			{
				result.FirstLighting = lighting;
			}
			if (!lighting.Hit)
			{
				break;
			}

			surface = BuildHitRayTracingPathSurface(hitSurface, path.DirectionWorld);
		}

		return result;
	}

}

#endif
