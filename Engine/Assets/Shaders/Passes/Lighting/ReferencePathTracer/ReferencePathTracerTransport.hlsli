#ifndef SPARKLE_REFERENCE_PATH_TRACER_TRANSPORT_HLSLI
#define SPARKLE_REFERENCE_PATH_TRACER_TRANSPORT_HLSLI

#include "/Engine/RayTracing/PathBsdf.hlsli"
#include "/Engine/RayTracing/PathTracer.hlsli"
#include "/Engine/RayTracing/RayEndpoints.hlsli"
#include "/Engine/RayTracing/RayTracingMaterialHit.hlsli"
#include "/Engine/RayTracing/RayTracingSceneTrace.hlsli"
#include "/Engine/Passes/Lighting/ReferencePathTracer/ReferencePathTracerDirectLighting.hlsli"
#include "/Engine/Passes/Lighting/ReferencePathTracer/ReferencePathTracerSampler.hlsli"

namespace ReferencePathTracer
{
	PathSample::DirectionSample SamplePathDirection(RayTracingPathSurface surface, PathBsdf::LobeMasses lobeMasses, SampleIdentity sampleIdentity, uint surfaceDepth)
	{
		const uint lobeChoiceDimension = SurfaceDimension(surfaceDepth, LobeChoiceOffset);
		const uint lobeChoiceWord = RandomWord(sampleIdentity, lobeChoiceDimension);
		const CommonRandom::CategoricalSample lobeChoice = CommonRandom::SampleCategorical(lobeChoiceWord, lobeMasses.Count);

		uint selectedLobe = PathSample::LobeSpecular;
		if (lobeMasses.Diffuse > 0.0f && lobeChoice.Index == 0u)
		{
			selectedLobe = PathSample::LobeDiffuse;
		}

		const uint directionXDimension = SurfaceDimension(surfaceDepth, BsdfDirectionXOffset);
		const uint directionYDimension = SurfaceDimension(surfaceDepth, BsdfDirectionYOffset);
		const uint directionXWord = RandomWord(sampleIdentity, directionXDimension);
		const uint directionYWord = RandomWord(sampleIdentity, directionYDimension);
		const float2 bsdfSample = float2(CommonRandom::OpenUnitInterval(directionXWord), CommonRandom::OpenUnitInterval(directionYWord));

		return PathBsdf::Sample(surface, lobeMasses, selectedLobe, bsdfSample, true, true);
	}

	bool SurvivesRussianRoulette(inout float3 throughput, SampleIdentity sampleIdentity, uint surfaceVertexCount)
	{
		const float maximumThroughput = max(throughput.r, max(throughput.g, throughput.b));
		const float targetSurvival = clamp(maximumThroughput, 0.05f, 0.95f);
		const float scaledSurvival = targetSurvival * 16777216.0f;
		const uint lowerThreshold = (uint)floor(scaledSurvival);
		const float thresholdFraction = scaledSurvival - (float)lowerThreshold;
		const bool bRoundTieUp = thresholdFraction == 0.5f && (lowerThreshold & 1u) != 0u;
		const bool bRoundUp = thresholdFraction > 0.5f || bRoundTieUp;
		const uint roundedThreshold = lowerThreshold + (bRoundUp ? 1u : 0u);
		const uint threshold = min(max(roundedThreshold, 1u), 16777215u);
		const uint rouletteDimension = SurfaceDimension(surfaceVertexCount - 1u, RouletteOffset);
		const uint rouletteValue = RandomWord(sampleIdentity, rouletteDimension) >> 8u;

		if (rouletteValue >= threshold)
		{
			return false;
		}

		PathTracer::ApplySurvivalCompensation(throughput, (float)threshold * 0x1.0p-24f);
		return true;
	}

	float3 TraceSurfaceTransport(RaytracingAccelerationStructure sceneTlas, Texture2D skyTexture, SamplerState skySampler, RayDesc traversal, SampleIdentity sampleIdentity)
	{
		const LightCounts lightCounts = GetLightCounts();
		PathTracer::PathState path;
		path.OriginWorld = traversal.Origin;
		path.DirectionWorld = traversal.Direction;
		path.Throughput = 1.0f.xxx;
		path.SurfaceDepth = 0u;

		PreviousPathEvent previousEvent;
		previousEvent.PositionWorld = 0.0f.xxx;
		previousEvent.BsdfPdfW = 0.0f;
		previousEvent.bDelta = true;

		float3 contribution = 0.0f.xxx;

		[loop]
		for (;;)
		{
			const RayTracingTraceResult trace = TraceSceneRay(sceneTlas, path.OriginWorld, path.DirectionWorld, traversal.TMin, traversal.TMax, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, 0xFFu);

			if (!trace.bHit)
			{
				const float3 environmentRadiance = EvaluateEnvironmentRadiance(lightCounts, previousEvent, path.DirectionWorld, skyTexture, skySampler);
				PathTracer::AddRadiance(contribution, path.Throughput, environmentRadiance);
				break;
			}

			const RayTracingHitSurfaceData hitSurface = ReconstructRayTracingHitSurface(trace);
			if (!hitSurface.bValid)
			{
				return asfloat(0x7FC00000u).xxx;
			}

			const RayTracingPathSurface surface = BuildHitRayTracingPathSurface(hitSurface, path.DirectionWorld);
			const uint surfaceVertexCount = path.SurfaceDepth + 1u;
			if (surfaceVertexCount > 4096u)
			{
				return asfloat(0x7FC00000u).xxx;
			}

			const float3 surfaceEmission = EvaluateSurfaceEmission(lightCounts, previousEvent, surface);
			PathTracer::AddRadiance(contribution, path.Throughput, surfaceEmission);

			const PathBsdf::LobeMasses lobeMasses = PathBsdf::BuildEqualLobeMasses(surface);

			const float3 directLighting = SampleDirectLighting(sceneTlas, skyTexture, skySampler, surface, lobeMasses, lightCounts, sampleIdentity, path.SurfaceDepth);
			PathTracer::AddRadiance(contribution, path.Throughput, directLighting);

			if (lobeMasses.Count == 0u)
			{
				break;
			}

			const PathSample::DirectionSample bsdf = SamplePathDirection(surface, lobeMasses, sampleIdentity, path.SurfaceDepth);
			if (!bsdf.bHasSupport)
			{
				break;
			}

			previousEvent.PositionWorld = surface.PositionWorld;
			previousEvent.BsdfPdfW = bsdf.PdfW;
			previousEvent.bDelta = bsdf.bDelta;

			PathTracer::ApplyDirectionSample(path, bsdf);

			const bool bRouletteTerminated = path.SurfaceDepth >= 3u && !SurvivesRussianRoulette(path.Throughput, sampleIdentity, surfaceVertexCount);

			if (bRouletteTerminated)
			{
				break;
			}

			traversal = RayEndpoints::Continuation(surface.PositionWorld, surface.GeometricNormalWorld, surface.PositionError, path.DirectionWorld);

			path.OriginWorld = traversal.Origin;
			path.DirectionWorld = traversal.Direction;
		}

		return contribution;
	}
}

#endif
