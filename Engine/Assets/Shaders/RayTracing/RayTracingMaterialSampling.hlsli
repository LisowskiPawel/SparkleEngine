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

bool ResolveRayTracingCandidateAlpha(uint instanceId, uint primitiveIndex, float2 barycentrics, bool bFrontFace)
{
	const RayTracingHitTriangle hitTriangle = LoadRayTracingHitTriangle(instanceId, primitiveIndex, barycentrics);
	if (!bFrontFace && (hitTriangle.Instance.Flags & HitSurface::InstanceFlagTwoSided) == 0u)
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

void ModulateRayTracingMaterialTexture(RayTracingHitMaterial material, uint textureSlot, float2 uv, float4 factor, inout float4 result, bool bEnabled = true)
{
	if (bEnabled && MaterialTextures::HasTexture(material.TextureFlags, textureSlot))
	{
		result = SampleRayTracingMaterialTexture(material, textureSlot, uv) * factor;
	}
}

void ModulateRayTracingMaterialTexture(RayTracingHitMaterial material, uint textureSlot, float2 uv, float factor, inout float result, bool bEnabled = true)
{
	if (bEnabled && MaterialTextures::HasTexture(material.TextureFlags, textureSlot))
	{
		result = SampleRayTracingMaterialTexture(material, textureSlot, uv).r * factor;
	}
}

void ModulateRayTracingMaterialTexture(RayTracingHitMaterial material, uint textureSlot, float2 uv, float3 factor, inout float3 result, bool bEnabled = true)
{
	if (bEnabled && MaterialTextures::HasTexture(material.TextureFlags, textureSlot))
	{
		result = SampleRayTracingMaterialTexture(material, textureSlot, uv).rgb * factor;
	}
}

void ApplyRayTracingMaterialNormal(RayTracingHitMaterial material, float2 uv, inout float3 normalTangent)
{
	if (MaterialTextures::HasTexture(material.TextureFlags, MaterialTextures::TextureSlotNormal))
	{
		const MaterialTextureMappingData mapping = material.TextureMappings[MaterialTextures::TextureSlotNormal];
		normalTangent = DecodeBc5TangentNormal(SampleRayTracingMaterialTexture(material, MaterialTextures::TextureSlotNormal, uv).rg, mapping.Strength);
	}
}

void ApplyRayTracingMaterialOcclusion(RayTracingHitMaterial material, float2 uv, inout float occlusion)
{
	if (MaterialTextures::HasTexture(material.TextureFlags, MaterialTextures::TextureSlotOcclusion))
	{
		const MaterialTextureMappingData mapping = material.TextureMappings[MaterialTextures::TextureSlotOcclusion];
		const float sampledOcclusion = SampleRayTracingMaterialTexture(material, MaterialTextures::TextureSlotOcclusion, uv).r;
		occlusion = lerp(1.0f, sampledOcclusion, mapping.Strength);
	}
}

RayTracingMaterialSample SampleRayTracingMaterial(RayTracingHitMaterial material, float2 uv, float4 vertexColor, bool bEvaluateEmissive)
{
	RayTracingMaterialSample result;
	result.BaseColor = material.BaseColor;
	result.Emissive = bEvaluateEmissive ? material.EmissiveColor : 0.0f.xxx;
	result.NormalTangent = float3(0.0f, 0.0f, 1.0f);
	result.SubsurfaceColor = material.SubsurfaceColor;
	result.Roughness = material.Roughness;
	result.Metallic = material.Metallic;
	result.AmbientOcclusion = 1.0f;
	result.SubsurfaceStrength = material.SubsurfaceStrength;

	ApplyRayTracingMaterialNormal(material, uv, result.NormalTangent);
	ModulateRayTracingMaterialTexture(material, MaterialTextures::TextureSlotBaseColor, uv, material.BaseColor, result.BaseColor);
	ModulateRayTracingMaterialTexture(material, MaterialTextures::TextureSlotRoughness, uv, material.Roughness, result.Roughness);
	ModulateRayTracingMaterialTexture(material, MaterialTextures::TextureSlotMetallic, uv, material.Metallic, result.Metallic);
	ModulateRayTracingMaterialTexture(material, MaterialTextures::TextureSlotEmissive, uv, material.EmissiveColor, result.Emissive, bEvaluateEmissive);
	ApplyRayTracingMaterialOcclusion(material, uv, result.AmbientOcclusion);
	ModulateRayTracingMaterialTexture(material, MaterialTextures::TextureSlotSubsurfaceColor, uv, material.SubsurfaceColor, result.SubsurfaceColor);
	ModulateRayTracingMaterialTexture(material, MaterialTextures::TextureSlotSubsurfaceStrength, uv, material.SubsurfaceStrength, result.SubsurfaceStrength);
	result.BaseColor *= vertexColor;
	return result;
}
