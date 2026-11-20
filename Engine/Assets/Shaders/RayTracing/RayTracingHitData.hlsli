#ifndef SPARKLE_RAY_TRACING_HIT_DATA_HLSLI
#define SPARKLE_RAY_TRACING_HIT_DATA_HLSLI

#include "/Engine/Resources/MaterialTextureMappingData.hlsli"

namespace RayTracingHitData
{
	static const uint InstanceFlagValid = 1u << 0u;
	static const uint InstanceFlagTwoSided = 1u << 2u;
	static const uint MaterialFlagEmissive = 1u << 5u;
}

struct RayTracingHitVertex
{
	float3 Position;
	float3 Normal;
	float4 Tangent;
	float2 TexCoord0;
	float4 Color;
};

struct RayTracingHitInstance
{
	uint FirstVertex;
	uint FirstIndex;
	uint IndexCount;
	uint MaterialSlot;
	uint Flags;
	uint MorphTargetDeltaOffset;
};

struct RayTracingHitMaterial
{
	float4 BaseColor;
	float3 EmissiveColor;
	float Metallic;
	float Roughness;
	float F0;
	float AlphaCutoff;
	uint AlphaMode;
	uint TextureFlags;
	float3 SubsurfaceColor;
	float SubsurfaceStrength;
	uint Flags;
	uint4 TextureIndices0;
	uint4 TextureIndices1;
	MaterialTextureMappingData TextureMappings[8];
};

StructuredBuffer<RayTracingHitVertex> RayTracingHitVertices : register(t4096, space1);
StructuredBuffer<uint> RayTracingHitIndices : register(t4097, space1);
StructuredBuffer<RayTracingHitInstance> RayTracingHitInstances : register(t4098, space1);
StructuredBuffer<RayTracingHitMaterial> RayTracingHitMaterials : register(t4099, space1);

struct RayTracingHitTriangle
{
	RayTracingHitInstance Instance;
	RayTracingHitMaterial Material;
	float3 BarycentricWeights;
	uint3 VertexIndices;
	RayTracingHitVertex V0;
	RayTracingHitVertex V1;
	RayTracingHitVertex V2;
};

RayTracingHitTriangle LoadRayTracingHitTriangle(uint instanceId, uint primitiveIndex, float2 barycentrics)
{
	RayTracingHitTriangle hitTriangle;
	hitTriangle.Instance = RayTracingHitInstances[instanceId];
	hitTriangle.Material = RayTracingHitMaterials[hitTriangle.Instance.MaterialSlot];
	hitTriangle.BarycentricWeights = float3(1.0f - barycentrics.x - barycentrics.y, barycentrics.x, barycentrics.y);
	const uint primitiveFirstIndex = hitTriangle.Instance.FirstIndex + primitiveIndex * 3u;
	const uint3 localVertexIndices = uint3(RayTracingHitIndices[primitiveFirstIndex], RayTracingHitIndices[primitiveFirstIndex + 1u], RayTracingHitIndices[primitiveFirstIndex + 2u]);

	hitTriangle.VertexIndices = hitTriangle.Instance.FirstVertex + localVertexIndices;
	hitTriangle.V0 = RayTracingHitVertices[hitTriangle.VertexIndices.x];
	hitTriangle.V1 = RayTracingHitVertices[hitTriangle.VertexIndices.y];
	hitTriangle.V2 = RayTracingHitVertices[hitTriangle.VertexIndices.z];
	return hitTriangle;
}

float2 InterpolateRayTracingHitTexCoord0(RayTracingHitTriangle hitTriangle)
{
	const float3 weights = hitTriangle.BarycentricWeights;
	return hitTriangle.V0.TexCoord0 * weights.x + hitTriangle.V1.TexCoord0 * weights.y + hitTriangle.V2.TexCoord0 * weights.z;
}

float4 InterpolateRayTracingHitColor(RayTracingHitTriangle hitTriangle)
{
	const float3 weights = hitTriangle.BarycentricWeights;
	return hitTriangle.V0.Color * weights.x + hitTriangle.V1.Color * weights.y + hitTriangle.V2.Color * weights.z;
}

#endif
