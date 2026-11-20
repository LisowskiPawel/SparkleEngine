#ifndef SPARKLE_REFERENCE_PATH_TRACER_LIGHT_SAMPLING_HLSLI
#define SPARKLE_REFERENCE_PATH_TRACER_LIGHT_SAMPLING_HLSLI

#include "/Engine/Common/Random.hlsli"
#include "/Engine/Lighting/DirectLightSampling.hlsli"
#include "/Engine/Lighting/EnvironmentSampling.hlsli"
#include "/Engine/RayTracing/PathLightSampling.hlsli"

namespace ReferencePathTracer
{
	static const uint KindAnalytic = 0u;
	static const uint KindEmissiveTriangle = 1u;
	static const uint KindEnvironment = 2u;

	struct LightCounts
	{
		uint Analytic;
		uint EmissiveTriangle;
		uint Environment;
		uint Total;
	};

	struct LightSelection
	{
		uint Kind;
		uint LightType;
		uint LightIndex;
		uint InstanceId;
		uint PrimitiveIndex;
		float Probability;
	};

	LightCounts GetLightCounts()
	{
		LightCounts result;
		result.Analytic = DirectLightSampling::GetDirectLightCount();
		result.EmissiveTriangle = PathLightSampling::CountEmissiveTriangles();
		result.Environment = SkyEnabled;
		result.Total = result.Analytic + result.EmissiveTriangle + result.Environment;
		return result;
	}

	LightSelection SelectLight(LightCounts counts, uint word)
	{
		LightSelection result = (LightSelection)0;
		const CommonRandom::CategoricalSample category = CommonRandom::SampleCategorical(word, counts.Total);
		result.Probability = category.ProbabilityMass;
		uint ordinal = category.Index;
		if (ordinal < counts.Analytic)
		{
			const DirectLightSampling::LightId light = DirectLightSampling::GetDirectLightId(ordinal);
			result.Kind = KindAnalytic;
			result.LightType = light.Type;
			result.LightIndex = light.Index;
			return result;
		}

		ordinal -= counts.Analytic;
		if (ordinal < counts.EmissiveTriangle)
		{
			result.Kind = KindEmissiveTriangle;
			PathLightSampling::FindEmissiveTriangle(ordinal, result.InstanceId, result.PrimitiveIndex);
			return result;
		}

		result.Kind = KindEnvironment;
		return result;
	}

	LightSampling::DirectLightSample SampleLight(LightSelection selection, float3 positionWorld, float2 shapeSample, Texture2D skyTexture, SamplerState skySampler)
	{
		LightSampling::DirectLightSample result;
		if (selection.Kind == KindAnalytic)
		{
			result = PathLightSampling::SampleAnalyticLight(selection.LightType, selection.LightIndex, positionWorld, shapeSample);
		}
		else if (selection.Kind == KindEmissiveTriangle)
		{
			result = PathLightSampling::SampleEmissiveTriangle(positionWorld, selection.InstanceId, selection.PrimitiveIndex, shapeSample);
		}
		else
		{
			result = SampleUniformEnvironmentRadiance(skyTexture, skySampler, shapeSample);
		}
		result.LightSelectionPdf = selection.Probability;
		return result;
	}

	float EnvironmentPdfW(LightCounts counts)
	{
		if (counts.Environment == 0u)
		{
			return 0.0f;
		}

		const uint environmentOrdinal = counts.Analytic + counts.EmissiveTriangle;
		const float selectionProbability = CommonRandom::CategoricalProbabilityMass(environmentOrdinal, counts.Total);
		return selectionProbability * 0.07957747154594766788f;
	}

	float EmissiveTrianglePdfW(LightCounts counts, float3 previousPositionWorld, RayTracingPathSurface surface)
	{
		const uint ordinal = PathLightSampling::GetEmissiveOrdinal(surface.InstanceId, surface.PrimitiveIndex);
		const float selectionProbability = CommonRandom::CategoricalProbabilityMass(counts.Analytic + ordinal, counts.Total);
		const float conditionalPdfW = PathLightSampling::EmissiveTrianglePdfW(previousPositionWorld, surface.PositionWorld, surface.InstanceId, surface.PrimitiveIndex);
		return selectionProbability * conditionalPdfW;
	}
}

#endif
