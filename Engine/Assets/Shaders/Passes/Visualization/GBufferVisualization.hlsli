#pragma once

#include "/Engine/Resources/RenderViewModeConstants.hlsli"
#include "/Engine/Common/Math.hlsli"

Texture2D GBufferBaseColor;
Texture2D GBufferWorldNormal;
Texture2D GBufferWorldTangent;
Texture2D GBufferMaterial;
Texture2D GBufferEmissive;
Texture2D GBufferSubsurface;

float3 VisualizeScalar(float value)
{
	return saturate(value).xxx;
}

float3 VisualizeWorldDirection(float3 directionWorld)
{
	return SafeNormalize(directionWorld) * 0.5f + 0.5f;
}

void StoreGBufferVisualization(uint2 pixelCoord, uint viewMode, RWTexture2D<float4> sceneColor)
{
	const int3 pixel = int3(pixelCoord, 0);
	const float4 baseColor = GBufferBaseColor.Load(pixel);
	float3 outputColor = 0.0f;

	switch (viewMode)
	{
		case RenderViewMode::GBufferDiffuse:
			outputColor = saturate(baseColor.rgb);
			break;

		case RenderViewMode::GBufferWorldNormal:
		{
			const float3 normalWorld = GBufferWorldNormal.Load(pixel).xyz;
			outputColor = VisualizeWorldDirection(normalWorld);
			break;
		}
		case RenderViewMode::GBufferWorldTangent:
		{
			const float4 tangent = GBufferWorldTangent.Load(pixel);
			outputColor = tangent.w > 0.0f ? VisualizeWorldDirection(tangent.xyz) : 0.0f.xxx;
			break;
		}

		case RenderViewMode::GBufferRoughness:
		{
			const float roughness = GBufferMaterial.Load(pixel).g;
			outputColor = VisualizeScalar(roughness);
			break;
		}

		case RenderViewMode::GBufferMetallic:
		{
			const float metallic = GBufferMaterial.Load(pixel).r;
			outputColor = VisualizeScalar(metallic);
			break;
		}

		case RenderViewMode::GBufferEmissive:
		{
			const float3 emissive = GBufferEmissive.Load(pixel).rgb;
			outputColor = max(emissive, 0.0f);
			break;
		}

		case RenderViewMode::GBufferAmbientOcclusion:
		{
			const float ambientOcclusion = GBufferMaterial.Load(pixel).b;
			outputColor = VisualizeScalar(ambientOcclusion);
			break;
		}

		case RenderViewMode::GBufferSubsurfaceColor:
		{
			const float3 subsurfaceColor = GBufferSubsurface.Load(pixel).rgb;
			outputColor = saturate(subsurfaceColor);
			break;
		}

		case RenderViewMode::GBufferSubsurfaceStrength:
		{
			const float subsurfaceStrength = GBufferSubsurface.Load(pixel).a;
			outputColor = VisualizeScalar(subsurfaceStrength);
			break;
		}
		default:
			return;
	}

	sceneColor[pixelCoord] = float4(outputColor, baseColor.a);
}
