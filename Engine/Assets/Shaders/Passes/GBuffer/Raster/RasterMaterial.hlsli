#pragma once

#include "/Engine/Resources/ObjectShaderData.hlsli"

#include "/Engine/Resources/Samplers.hlsli"
#include "/Engine/Geometry/Basis.hlsli"
#include "/Engine/Geometry/PixelInput.hlsli"
#include "/Engine/Material/MaterialAlpha.hlsli"
#include "/Engine/Material/MaterialNormal.hlsli"
#include "/Engine/Material/MaterialTextureSlots.hlsli"

Texture2D TextureBaseColor;
Texture2D TextureNormal;
Texture2D TextureRoughness;
Texture2D TextureMetallic;
Texture2D TextureOcclusion;
Texture2D TextureEmissive;
Texture2D TextureSubsurfaceColor;
Texture2D TextureSubsurfaceStrength;

namespace RasterMaterial
{
	void ApplyAlphaMode(float alpha)
	{
		if (AlphaMode == MaterialAlpha::Mask)
		{
			clip(alpha - AlphaCutoff);
		}
	}

	struct Properties
	{
		float3 BaseColor;
		float3 NormalWorld;
		float Roughness;
		float Metallic;
		float DielectricF0;
		float AmbientOcclusion;

		float3 SubsurfaceColor;
		float SubsurfaceStrength;

		float3 Emissive;
		float Alpha;
		uint AlphaMode;
	};

	float4 SampleBaseColor(float2 uv)
	{
		return MaterialTextures::HasTexture(TextureFlags, MaterialTextures::TextureSlotBaseColor) ? TextureBaseColor.Sample(SamplerAniso16xWrap, uv) * BaseColor : BaseColor;
	}

	float3 SampleNormalTangent(float2 uv)
	{
		if (!MaterialTextures::HasTexture(TextureFlags, MaterialTextures::TextureSlotNormal))
		{
			return float3(0.0f, 0.0f, 1.0f);
		}

		return DecodeBc5TangentNormal(TextureNormal.Sample(SamplerAniso16xWrap, uv).rg, NormalScale);
	}

	float SampleRoughness(float2 uv)
	{
		if (!MaterialTextures::HasTexture(TextureFlags, MaterialTextures::TextureSlotRoughness))
		{
			return Roughness;
		}

		return TextureRoughness.Sample(SamplerAniso16xWrap, uv).r * Roughness;
	}

	float3 SampleEmissive(float2 uv)
	{
		if (EmissiveEnabled == 0u)
		{
			return 0.0f.xxx;
		}
		if (!MaterialTextures::HasTexture(TextureFlags, MaterialTextures::TextureSlotEmissive))
		{
			return EmissiveColor;
		}

		return TextureEmissive.Sample(SamplerAniso16xWrap, uv).rgb * EmissiveColor;
	}

	float SampleMetallic(float2 uv)
	{
		if (!MaterialTextures::HasTexture(TextureFlags, MaterialTextures::TextureSlotMetallic))
		{
			return Metallic;
		}

		return TextureMetallic.Sample(SamplerAniso16xWrap, uv).r * Metallic;
	}

	float SampleAmbientOcclusion(float2 uv)
	{
		if (!MaterialTextures::HasTexture(TextureFlags, MaterialTextures::TextureSlotOcclusion))
		{
			return 1.0f;
		}

		return TextureOcclusion.Sample(SamplerAniso16xWrap, uv).r;
	}

	float3 SampleSubsurfaceColor(float2 uv)
	{
		if (!MaterialTextures::HasTexture(TextureFlags, MaterialTextures::TextureSlotSubsurfaceColor))
		{
			return SubsurfaceColor;
		}

		return TextureSubsurfaceColor.Sample(SamplerAniso16xWrap, uv).rgb * SubsurfaceColor;
	}

	float SampleSubsurfaceStrength(float2 uv)
	{
		if (!MaterialTextures::HasTexture(TextureFlags, MaterialTextures::TextureSlotSubsurfaceStrength))
		{
			return SubsurfaceStrength;
		}

		return TextureSubsurfaceStrength.Sample(SamplerAniso16xWrap, uv).r * SubsurfaceStrength;
	}

	Properties Sample(PS::Input input)
	{
		Properties properties;
		const float4 baseColor = SampleBaseColor(input.TexCoord);
		properties.BaseColor = baseColor.rgb;
		properties.Alpha = baseColor.a;
		properties.AlphaMode = AlphaMode;
		ApplyAlphaMode(properties.Alpha);

		const float3 normalTangent = SampleNormalTangent(input.TexCoord);
		const float tangentSign = input.TangentWorld.w >= 0.0f ? 1.0f : -1.0f;
		const float3 bitangentWorld = ComputeBitangentFromSign(input.NormalWorld, input.TangentWorld.xyz, tangentSign);
		properties.NormalWorld = TransformTangentNormalToWorld(normalTangent, input.NormalWorld, input.TangentWorld.xyz, bitangentWorld);
		if (!input.bIsFrontFace)
		{
			properties.NormalWorld = -properties.NormalWorld;
		}

		properties.Roughness = SampleRoughness(input.TexCoord);
		properties.Metallic = SampleMetallic(input.TexCoord);
		properties.DielectricF0 = saturate(F0);
		properties.AmbientOcclusion = SampleAmbientOcclusion(input.TexCoord);
		properties.SubsurfaceColor = SampleSubsurfaceColor(input.TexCoord);
		properties.SubsurfaceStrength = SampleSubsurfaceStrength(input.TexCoord);
		properties.Emissive = SampleEmissive(input.TexCoord);
		return properties;
	}
}
