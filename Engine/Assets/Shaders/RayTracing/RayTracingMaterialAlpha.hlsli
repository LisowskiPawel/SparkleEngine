#pragma once

#include "/Engine/Material/MaterialTextureTable.hlsli"
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
