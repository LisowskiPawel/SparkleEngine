#pragma once

#include "/Engine/Common/Sampling.hlsli"

float3 OrthonormalizeTangent(float3 tangent, float3 normal)
{
	const float3 projectedTangent = tangent - normal * dot(tangent, normal);
	const float lengthSquared = dot(projectedTangent, projectedTangent);
	return lengthSquared > 0.0f ? projectedTangent * rsqrt(lengthSquared) : CommonSampling::BuildOrthonormalBasis(normal).Tangent;
}

float3 ComputeBitangentFromSign(float3 normal, float3 tangent, float tangentSign)
{
	return normalize(tangentSign * cross(normal, tangent));
}

float3 TransformTangentNormalToWorld(float3 normalTangent, float3 normalWorld, float3 tangentWorld, float3 bitangentWorld)
{
	const float3x3 TBN = float3x3(tangentWorld, bitangentWorld, normalWorld);
	return normalize(mul(normalTangent, TBN));
}
