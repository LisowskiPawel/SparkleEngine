#ifndef SPARKLE_RAY_TRACING_PATH_SAMPLING_HLSLI
#define SPARKLE_RAY_TRACING_PATH_SAMPLING_HLSLI

#include "/Engine/Common/Random.hlsli"
#include "/Engine/Common/Color.hlsli"
#include "/Engine/Lighting/SurfaceLighting.hlsli"
#include "/Engine/RayTracing/PathBsdf.hlsli"
#include "/Engine/RayTracing/PathSurface.hlsli"
#include "/Engine/RayTracing/RayTracingPathSample.hlsli"

namespace PathSampling
{
	static const uint SpecularSampleModeStochasticGGX = 1u;

	struct RandomSamples
	{
		float Lobe;
		float2 Direction;
		float Roulette;
	};

	RandomSamples GenerateRandomSamples(uint2 pixelCoord, uint bounceIndex, uint sampleIndex, uint randomFrameIndex)
	{
		const uint4 words = CommonRandom::Philox4x32(uint4(pixelCoord, randomFrameIndex, sampleIndex), uint2(bounceIndex, 0x4C495450u));

		RandomSamples result;
		result.Lobe = CommonRandom::OpenUnitInterval(words.x);
		result.Direction = float2(CommonRandom::OpenUnitInterval(words.y), CommonRandom::OpenUnitInterval(words.z));
		result.Roulette = CommonRandom::OpenUnitInterval(words.w);
		return result;
	}

	float DiffuseLobeProbability(RayTracingPathSurface surface, float3 f0)
	{
		const float diffuseWeight = max(CommonColor::LuminanceRec709(surface.BaseColor) * (1.0f - surface.Metallic), 0.0f);
		const float specularWeight = max(CommonColor::LuminanceRec709(f0), 0.0f);
		const float totalWeight = diffuseWeight + specularWeight;
		return totalWeight > 0.0f ? saturate(diffuseWeight / totalWeight) : 0.5f;
	}

	PathSample::DirectionSample InvalidSample()
	{
		PathSample::DirectionSample result = (PathSample::DirectionSample)0;
		return result;
	}

	PathSample::DirectionSample SampleBSDF(RayTracingPathSurface surface, uint specularSampleMode, RandomSamples randomSamples, bool bEvaluateDiffuse, bool bEvaluateSpecular)
	{
		if (!surface.bValid)
		{
			return InvalidSample();
		}

		const float3 f0 = SurfaceLighting::BuildF0(surface.BaseColor, surface.Metallic, surface.DielectricF0);
		const float diffuseMass = DiffuseLobeProbability(surface, f0);
		PathBsdf::LobeMasses masses;
		masses.Diffuse = diffuseMass;
		masses.Specular = 1.0f - diffuseMass;
		masses.Count = (masses.Diffuse > 0.0f ? 1u : 0u) + (masses.Specular > 0.0f ? 1u : 0u);
		masses.bSpecularDelta = surface.Roughness == 0.0f;
		if (specularSampleMode != SpecularSampleModeStochasticGGX && !masses.bSpecularDelta)
		{
			return InvalidSample();
		}

		uint selectedLobe = PathSample::LobeSpecular;
		if (randomSamples.Lobe < diffuseMass)
		{
			selectedLobe = PathSample::LobeDiffuse;
		}

		return PathBsdf::Sample(surface, masses, selectedLobe, randomSamples.Direction, bEvaluateDiffuse, bEvaluateSpecular);
	}

	float RussianRouletteSurvivalProbability(float3 throughput, uint bounceIndex)
	{
		if (bounceIndex < 2u)
		{
			return 1.0f;
		}

		return saturate(max(max(throughput.r, throughput.g), throughput.b));
	}
}

#endif
