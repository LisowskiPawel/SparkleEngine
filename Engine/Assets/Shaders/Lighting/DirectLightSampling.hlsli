#ifndef SPARKLE_DIRECT_LIGHT_SAMPLING_HLSLI
#define SPARKLE_DIRECT_LIGHT_SAMPLING_HLSLI

#include "/Engine/Resources/LightGpuData.hlsli"
#include "/Engine/Resources/SceneLightingUniformData.hlsli"

#include "/Engine/Lighting/AreaLights.hlsli"

namespace DirectLightSampling
{
	struct LightId
	{
		uint Type;
		uint Index;
	};

	struct LightCandidate
	{
		LightId Light;
		float SelectionPdf;
		float Valid;
	};

	LightCandidate InvalidLightCandidate()
	{
		LightCandidate candidate;
		candidate.Light.Type = LightSampling::LightTypeInvalid;
		candidate.Light.Index = LightSampling::LightIndexInvalid;
		candidate.SelectionPdf = 0.0f;
		candidate.Valid = 0.0f;
		return candidate;
	}

	bool IsValid(LightCandidate candidate)
	{
		return candidate.Valid > 0.5f && candidate.SelectionPdf > 0.0f;
	}

	bool IsLightIdInRange(LightId light)
	{
		if (light.Type == LightSampling::LightTypeDirectional)
		{
			return light.Index < SceneLighting.DirectionalLightCount;
		}

		if (light.Type == LightSampling::LightTypePoint)
		{
			return light.Index < SceneLighting.PointLightCount;
		}

		if (light.Type == LightSampling::LightTypeSpot)
		{
			return light.Index < SceneLighting.SpotLightCount;
		}

		if (light.Type == LightSampling::LightTypeRect)
		{
			return light.Index < SceneLighting.RectLightCount;
		}

		return false;
	}

	uint GetDirectLightCount()
	{
		return SceneLighting.DirectionalLightCount + SceneLighting.PointLightCount + SceneLighting.SpotLightCount + SceneLighting.RectLightCount;
	}

	LightId GetDirectLightId(uint linearIndex)
	{
		LightId light;
		if (linearIndex < SceneLighting.DirectionalLightCount)
		{
			light.Type = LightSampling::LightTypeDirectional;
			light.Index = linearIndex;
			return light;
		}

		linearIndex -= SceneLighting.DirectionalLightCount;
		if (linearIndex < SceneLighting.PointLightCount)
		{
			light.Type = LightSampling::LightTypePoint;
			light.Index = linearIndex;
			return light;
		}

		linearIndex -= SceneLighting.PointLightCount;
		if (linearIndex < SceneLighting.SpotLightCount)
		{
			light.Type = LightSampling::LightTypeSpot;
			light.Index = linearIndex;
			return light;
		}

		linearIndex -= SceneLighting.SpotLightCount;
		if (linearIndex < SceneLighting.RectLightCount)
		{
			light.Type = LightSampling::LightTypeRect;
			light.Index = linearIndex;
			return light;
		}

		light.Type = LightSampling::LightTypeInvalid;
		light.Index = LightSampling::LightIndexInvalid;
		return light;
	}

	LightCandidate BuildLightCandidate(LightId light, float selectionPdf)
	{
		LightCandidate candidate;
		candidate.Light = light;
		candidate.SelectionPdf = selectionPdf;
		candidate.Valid = IsLightIdInRange(light) && selectionPdf > 0.0f ? 1.0f : 0.0f;
		if (IsValid(candidate))
		{
			return candidate;
		}

		return InvalidLightCandidate();
	}

	bool CastsShadow(LightId light)
	{
		if (light.Type == LightSampling::LightTypeDirectional)
		{
			return DirectionalLights[light.Index].CastShadow != 0u;
		}

		if (light.Type == LightSampling::LightTypePoint)
		{
			return PointLights[light.Index].CastShadow != 0u;
		}

		if (light.Type == LightSampling::LightTypeSpot)
		{
			return SpotLights[light.Index].CastShadow != 0u;
		}

		if (light.Type == LightSampling::LightTypeRect)
		{
			return RectLights[light.Index].CastShadow != 0u;
		}

		return false;
	}

	LightCandidate SampleUniformLightCandidate(float random)
	{
		const uint lightCount = GetDirectLightCount();
		if (lightCount == 0u)
		{
			return InvalidLightCandidate();
		}

		const uint linearIndex = min((uint)(saturate(random) * (float)lightCount), lightCount - 1u);
		return BuildLightCandidate(GetDirectLightId(linearIndex), rcp((float)lightCount));
	}

	LightSampling::DirectLightSample SampleDirectLight(LightCandidate candidate, float3 positionWorld, float2 shapeSample)
	{
		if (!IsValid(candidate))
		{
			return LightSampling::InvalidDirectLightSample();
		}

		LightSampling::DirectLightSample sample = LightSampling::InvalidDirectLightSample();
		if (candidate.Light.Type == LightSampling::LightTypeDirectional)
		{
			sample = AreaLights::SampleDirectionalLight(candidate.Light.Index, shapeSample);
		}
		else if (candidate.Light.Type == LightSampling::LightTypePoint)
		{
			sample = AreaLights::SamplePointLight(positionWorld, candidate.Light.Index, shapeSample);
		}
		else if (candidate.Light.Type == LightSampling::LightTypeSpot)
		{
			sample = AreaLights::SampleSpotLight(positionWorld, candidate.Light.Index, shapeSample);
		}
		else if (candidate.Light.Type == LightSampling::LightTypeRect)
		{
			sample = AreaLights::SampleRectLight(positionWorld, candidate.Light.Index, shapeSample);
		}

		sample.LightSelectionPdf = max(candidate.SelectionPdf, 1.0e-6f);
		return sample;
	}

}

#endif
