#pragma once

#include "/Engine/Resources/ObjectShaderData.hlsli"

#include "/Engine/Resources/Samplers.hlsli"
#include "/Engine/Geometry/Basis.hlsli"
#include "/Engine/Geometry/PixelInput.hlsli"
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

namespace Material
{
	static const uint AlphaModeOpaque = 0u;
	static const uint AlphaModeMask = 1u;
	static const uint AlphaModeBlend = 2u;

	void ApplyAlphaMode(float alpha)
	{
		if (AlphaMode == AlphaModeMask)
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

	float4 SampleBaseColor(float2 UV)
	{
		return MaterialTextures::HasTexture(TextureFlags, MaterialTextures::TextureSlotBaseColor) ? TextureBaseColor.Sample(SamplerAniso16xWrap, UV) * BaseColor : BaseColor;
	}

	float3 SampleNormalTangent(float2 UV)
	{
		if (!MaterialTextures::HasTexture(TextureFlags, MaterialTextures::TextureSlotNormal))
		{
			return float3(0.0f, 0.0f, 1.0f);
		}

		return DecodeBc5TangentNormal(TextureNormal.Sample(SamplerAniso16xWrap, UV).rg, NormalScale);
	}

	float SampleRoughness(float2 UV)
	{
		if (!MaterialTextures::HasTexture(TextureFlags, MaterialTextures::TextureSlotRoughness))
		{
			return Roughness;
		}

		return TextureRoughness.Sample(SamplerAniso16xWrap, UV).r * Roughness;
	}

	float3 SampleEmissive(float2 UV)
	{
		if (EmissiveEnabled == 0u)
		{
			return 0.0f.xxx;
		}
		if (!MaterialTextures::HasTexture(TextureFlags, MaterialTextures::TextureSlotEmissive))
		{
			return EmissiveColor;
		}

		return TextureEmissive.Sample(SamplerAniso16xWrap, UV).rgb * EmissiveColor;
	}

	float SampleMetallic(float2 UV)
	{
		if (!MaterialTextures::HasTexture(TextureFlags, MaterialTextures::TextureSlotMetallic))
		{
			return Metallic;
		}

		return TextureMetallic.Sample(SamplerAniso16xWrap, UV).r * Metallic;
	}

	float SampleAmbientOcclusion(float2 UV)
	{
		if (!MaterialTextures::HasTexture(TextureFlags, MaterialTextures::TextureSlotOcclusion))
		{
			return 1.0f;
		}

		return TextureOcclusion.Sample(SamplerAniso16xWrap, UV).r;
	}

	float3 SampleSubsurfaceColor(float2 UV)
	{
		if (!MaterialTextures::HasTexture(TextureFlags, MaterialTextures::TextureSlotSubsurfaceColor))
		{
			return SubsurfaceColor;
		}

		return TextureSubsurfaceColor.Sample(SamplerAniso16xWrap, UV).rgb * SubsurfaceColor;
	}

	float SampleSubsurfaceStrength(float2 UV)
	{
		if (!MaterialTextures::HasTexture(TextureFlags, MaterialTextures::TextureSlotSubsurfaceStrength))
		{
			return SubsurfaceStrength;
		}

		return TextureSubsurfaceStrength.Sample(SamplerAniso16xWrap, UV).r * SubsurfaceStrength;
	}

	Properties Sample(PS::Input Input)
	{
		Properties props;
		const float4 baseColor = SampleBaseColor(Input.TexCoord);
		props.BaseColor = baseColor.rgb;
		props.Alpha = baseColor.a;
		props.AlphaMode = AlphaMode;
		ApplyAlphaMode(props.Alpha);

		const float3 normalTangent = SampleNormalTangent(Input.TexCoord);
		const float tangentSign = Input.TangentWorld.w >= 0.0f ? 1.0f : -1.0f;
		const float3 bitangentWorld = ComputeBitangentFromSign(Input.NormalWorld, Input.TangentWorld.xyz, tangentSign);
		props.NormalWorld = TransformTangentNormalToWorld(normalTangent, Input.NormalWorld, Input.TangentWorld.xyz, bitangentWorld);
		if (!Input.bIsFrontFace)
		{
			props.NormalWorld = -props.NormalWorld;
		}

		props.Roughness = SampleRoughness(Input.TexCoord);
		props.Metallic = SampleMetallic(Input.TexCoord);
		props.DielectricF0 = saturate(F0);
		props.AmbientOcclusion = SampleAmbientOcclusion(Input.TexCoord);
		props.SubsurfaceColor = SampleSubsurfaceColor(Input.TexCoord);
		props.SubsurfaceStrength = SampleSubsurfaceStrength(Input.TexCoord);
		props.Emissive = SampleEmissive(Input.TexCoord);
		return props;
	}
}
