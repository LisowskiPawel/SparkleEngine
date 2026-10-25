#ifndef SPARKLE_SURFACE_LIGHTING_HLSLI
#define SPARKLE_SURFACE_LIGHTING_HLSLI

#include "/Engine/BRDF/BRDF.hlsli"
#include "/Engine/Lighting/LightSampling.hlsli"

namespace SurfaceLighting
{
	struct Surface
	{
		float3 ViewDirWorld;
		float3 NormalWorld;
		float3 BaseColor;
		float Roughness;
		float Metallic;
		float DielectricF0;
		float3 SubsurfaceColor;
		float SubsurfaceStrength;
		bool EvaluateDiffuse;
		bool EvaluateSpecular;
		bool EvaluateSubsurface;
	};

	struct Contribution
	{
		float3 Diffuse;
		float3 Specular;
		float3 Subsurface;
	};

	float3 BuildF0(float3 baseColor, float metallic, float dielectricF0)
	{
		return lerp(saturate(dielectricF0).xxx, saturate(baseColor), saturate(metallic));
	}

	Contribution EvaluateDirectLightSample(Surface surface, LightSampling::DirectLightSample lightSample, float shadowVisibility)
	{
		Contribution result = (Contribution)0;
		if (!lightSample.Valid || shadowVisibility <= 0.0f)
		{
			return result;
		}

		const float samplePdf = lightSample.PdfW * lightSample.LightSelectionPdf;
		if (samplePdf <= 0.0f)
		{
			return result;
		}

		const float3 f0 = BuildF0(surface.BaseColor, surface.Metallic, surface.DielectricF0);
		const BRDF::ShadingData shadingData = BRDF::ComputeShadingData(surface.NormalWorld, surface.ViewDirWorld, lightSample.DirectionWorld);
		if (shadingData.NoL <= 0.0f || shadingData.NoV <= 0.0f)
		{
			return result;
		}

		const BRDF::DirectResponse response = BRDF::EvaluateDirect(shadingData,
		                                                           surface.BaseColor,
		                                                           surface.Roughness,
		                                                           surface.Metallic,
		                                                           f0,
		                                                           surface.SubsurfaceColor,
		                                                           surface.SubsurfaceStrength,
		                                                           surface.EvaluateDiffuse,
		                                                           surface.EvaluateSpecular,
		                                                           surface.EvaluateSubsurface);

		const float3 radiance = lightSample.IncidentRadiance * shadowVisibility / samplePdf;
		const float3 incidentRadiance = radiance * shadingData.NoL;
		result.Diffuse = response.Diffuse * incidentRadiance;
		result.Specular = response.Specular * incidentRadiance;
		result.Subsurface = response.Subsurface * incidentRadiance;

		return result;
	}
}

#endif
