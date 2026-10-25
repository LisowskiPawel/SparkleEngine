#pragma once

#include "/Engine/Material/MaterialTextureTable.hlsli"
#include "/Engine/Material/MaterialNormal.hlsli"
#include "/Engine/RayTracing/RayTracingHitData.hlsli"

Texture2D MaterialTextureTable[4096] : register(t0, space1);

float4 SampleRayTracingMaterialTexture(RayTracingHitMaterial material, uint textureSlot, float2 uv)
{
	const uint textureIndex = MaterialTextures::ResolveTextureIndex(material.TextureIndices0, material.TextureIndices1, textureSlot);
	const MaterialTextureMappingData mapping = material.TextureMappings[textureSlot];
	const float2 mappedUv = MaterialTextures::TransformUv(uv, mapping.UvLinear, mapping.UvOffset);
	return MaterialTextures::SampleBaseLevelBilinear(MaterialTextureTable, textureIndex, mappedUv, mapping.AddressModes);
}

bool PassesRayTracingMaterialAlpha(RayTracingHitMaterial material, float2 texCoord0, float4 vertexColor)
{
	if (material.AlphaMode != HitSurface::AlphaModeTested)
	{
		return true;
	}

	float alpha = material.BaseColor.a;
	if (MaterialTextures::HasTexture(material.TextureFlags, MaterialTextures::TextureSlotBaseColor))
	{
		alpha *= SampleRayTracingMaterialTexture(material, MaterialTextures::TextureSlotBaseColor, texCoord0).a;
	}
	return alpha * vertexColor.a >= material.AlphaCutoff;
}

bool ResolveRayTracingCandidateAlpha(uint instanceId, uint primitiveIndex, float2 barycentrics, bool frontFace)
{
	const RayTracingHitTriangle hitTriangle = LoadRayTracingHitTriangle(instanceId, primitiveIndex, barycentrics);
	if (!frontFace && (hitTriangle.Instance.Flags & HitSurface::InstanceFlagTwoSided) == 0u)
	{
		return false;
	}

	return PassesRayTracingMaterialAlpha(hitTriangle.Material, InterpolateRayTracingHitTexCoord0(hitTriangle), InterpolateRayTracingHitColor(hitTriangle));
}

struct RayTracingMaterialSample
{
	float4 BaseColor;
	float3 Emissive;
	float3 NormalTangent;
	float3 SubsurfaceColor;
	float Roughness;
	float Metallic;
	float AmbientOcclusion;
	float SubsurfaceStrength;
};

RayTracingMaterialSample SampleRayTracingMaterial(RayTracingHitMaterial material, float2 uv, float4 vertexColor, bool evaluateEmissive)
{
	RayTracingMaterialSample result;
	result.BaseColor = material.BaseColor;
	result.Emissive = evaluateEmissive ? material.EmissiveColor : 0.0f.xxx;
	result.NormalTangent = float3(0.0f, 0.0f, 1.0f);
	result.SubsurfaceColor = material.SubsurfaceColor;
	result.Roughness = material.Roughness;
	result.Metallic = material.Metallic;
	result.AmbientOcclusion = 1.0f;
	result.SubsurfaceStrength = material.SubsurfaceStrength;

	if (MaterialTextures::HasTexture(material.TextureFlags, MaterialTextures::TextureSlotNormal))
	{
		const MaterialTextureMappingData mapping = material.TextureMappings[MaterialTextures::TextureSlotNormal];

		result.NormalTangent = DecodeBc5TangentNormal(SampleRayTracingMaterialTexture(material, MaterialTextures::TextureSlotNormal, uv).rg, mapping.Strength);
	}

	if (MaterialTextures::HasTexture(material.TextureFlags, MaterialTextures::TextureSlotBaseColor))
	{
		result.BaseColor = SampleRayTracingMaterialTexture(material, MaterialTextures::TextureSlotBaseColor, uv) * material.BaseColor;
	}

	if (MaterialTextures::HasTexture(material.TextureFlags, MaterialTextures::TextureSlotRoughness))
	{
		result.Roughness = SampleRayTracingMaterialTexture(material, MaterialTextures::TextureSlotRoughness, uv).r * material.Roughness;
	}

	if (MaterialTextures::HasTexture(material.TextureFlags, MaterialTextures::TextureSlotMetallic))
	{
		result.Metallic = SampleRayTracingMaterialTexture(material, MaterialTextures::TextureSlotMetallic, uv).r * material.Metallic;
	}

	if (evaluateEmissive && MaterialTextures::HasTexture(material.TextureFlags, MaterialTextures::TextureSlotEmissive))
	{
		result.Emissive = SampleRayTracingMaterialTexture(material, MaterialTextures::TextureSlotEmissive, uv).rgb * material.EmissiveColor;
	}

	if (MaterialTextures::HasTexture(material.TextureFlags, MaterialTextures::TextureSlotOcclusion))
	{
		const MaterialTextureMappingData mapping = material.TextureMappings[MaterialTextures::TextureSlotOcclusion];
		const float sampledOcclusion = SampleRayTracingMaterialTexture(material, MaterialTextures::TextureSlotOcclusion, uv).r;

		result.AmbientOcclusion = lerp(1.0f, sampledOcclusion, mapping.Strength);
	}

	if (MaterialTextures::HasTexture(material.TextureFlags, MaterialTextures::TextureSlotSubsurfaceColor))
	{
		result.SubsurfaceColor = SampleRayTracingMaterialTexture(material, MaterialTextures::TextureSlotSubsurfaceColor, uv).rgb * material.SubsurfaceColor;
	}

	if (MaterialTextures::HasTexture(material.TextureFlags, MaterialTextures::TextureSlotSubsurfaceStrength))
	{
		result.SubsurfaceStrength = SampleRayTracingMaterialTexture(material, MaterialTextures::TextureSlotSubsurfaceStrength, uv).r * material.SubsurfaceStrength;
	}

	result.BaseColor *= vertexColor;
	return result;
}
