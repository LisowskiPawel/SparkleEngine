#ifndef SPARKLE_RAY_TRACING_EVALUATED_TRIANGLE_HLSLI
#define SPARKLE_RAY_TRACING_EVALUATED_TRIANGLE_HLSLI

#include "/Engine/Resources/MeshInstanceShaderData.hlsli"

#include "/Engine/Geometry/Basis.hlsli"
#include "/Engine/Geometry/Morphing.hlsli"
#include "/Engine/Geometry/Skinning.hlsli"
#include "/Engine/RayTracing/RayTracingHitData.hlsli"

struct RayTracingEvaluatedVertex
{
	float3 Position;
	float3 Normal;
	float3 Tangent;
	float TangentSign;
	float2 TexCoord0;
	float4 Color;
};

struct RayTracingEvaluatedTriangle
{
	RayTracingHitInstance Instance;
	RayTracingHitMaterial Material;
	float3 BarycentricWeights;
	RayTracingEvaluatedVertex V0;
	RayTracingEvaluatedVertex V1;
	RayTracingEvaluatedVertex V2;
};

RayTracingEvaluatedVertex EvaluateRayTracingVertex(RayTracingHitVertex vertex,
                                                   MeshInstanceData mesh,
                                                   RayTracingHitInstance instance,
                                                   uint vertexIndex,
                                                   StructuredBuffer<float> weights,
                                                   StructuredBuffer<JointMatrixData> joints)
{
	const uint localVertexIndex = vertexIndex - instance.FirstVertex;
	const MorphedVertexAttributes morphed = ApplyMorphing(mesh, instance.MorphTargetDeltaOffset, localVertexIndex, vertex.Position, vertex.Normal, vertex.Tangent.xyz, weights);
	const SkinnedVertexAttributes skinned = ApplySkinning(mesh, vertexIndex, morphed.Position, morphed.Normal, morphed.Tangent, joints);

	RayTracingEvaluatedVertex result;
	result.Position = skinned.Position;
	result.Normal = skinned.Normal;
	result.Tangent = skinned.Tangent;
	result.TangentSign = vertex.Tangent.w;
	result.TexCoord0 = vertex.TexCoord0;
	result.Color = vertex.Color;
	return result;
}

RayTracingEvaluatedTriangle EvaluateRayTracingTriangle(uint instanceId, uint primitiveIndex, float2 barycentrics)
{
	const RayTracingHitTriangle source = LoadRayTracingHitTriangle(instanceId, primitiveIndex, barycentrics);
	const MeshInstanceData mesh = MeshInstances[instanceId];
	RayTracingEvaluatedTriangle result;
	result.Instance = source.Instance;
	result.Material = source.Material;
	result.BarycentricWeights = source.BarycentricWeights;
	result.V0 = EvaluateRayTracingVertex(source.V0, mesh, source.Instance, source.VertexIndices.x, MorphWeights, JointMatrices);
	result.V1 = EvaluateRayTracingVertex(source.V1, mesh, source.Instance, source.VertexIndices.y, MorphWeights, JointMatrices);
	result.V2 = EvaluateRayTracingVertex(source.V2, mesh, source.Instance, source.VertexIndices.z, MorphWeights, JointMatrices);
	return result;
}

float3 InterpolateRayTracingPosition(RayTracingEvaluatedTriangle evaluatedTriangle)
{
	precise float3 edge1 = evaluatedTriangle.V1.Position - evaluatedTriangle.V0.Position;
	precise float3 edge2 = evaluatedTriangle.V2.Position - evaluatedTriangle.V0.Position;
	precise float3 position = evaluatedTriangle.V0.Position + mad(evaluatedTriangle.BarycentricWeights.y, edge1, evaluatedTriangle.BarycentricWeights.z * edge2);
	return position;
}

float3 EvaluatePreviousRayTracingPosition(RayTracingHitTriangle hitTriangle, MeshInstanceData mesh)
{
	const float3 p0 = EvaluateRayTracingVertex(hitTriangle.V0, mesh, hitTriangle.Instance, hitTriangle.VertexIndices.x, PreviousMorphWeights, PreviousJointMatrices).Position;
	const float3 p1 = EvaluateRayTracingVertex(hitTriangle.V1, mesh, hitTriangle.Instance, hitTriangle.VertexIndices.y, PreviousMorphWeights, PreviousJointMatrices).Position;
	const float3 p2 = EvaluateRayTracingVertex(hitTriangle.V2, mesh, hitTriangle.Instance, hitTriangle.VertexIndices.z, PreviousMorphWeights, PreviousJointMatrices).Position;
	precise float3 edge1 = p1 - p0;
	precise float3 edge2 = p2 - p0;
	precise float3 position = p0 + mad(hitTriangle.BarycentricWeights.y, edge1, hitTriangle.BarycentricWeights.z * edge2);
	return position;
}

float2 InterpolateRayTracingTexCoord0(RayTracingEvaluatedTriangle evaluatedTriangle)
{
	return evaluatedTriangle.V0.TexCoord0 * evaluatedTriangle.BarycentricWeights.x + evaluatedTriangle.V1.TexCoord0 * evaluatedTriangle.BarycentricWeights.y
	    + evaluatedTriangle.V2.TexCoord0 * evaluatedTriangle.BarycentricWeights.z;
}

float4 InterpolateRayTracingColor(RayTracingEvaluatedTriangle evaluatedTriangle)
{
	return evaluatedTriangle.V0.Color * evaluatedTriangle.BarycentricWeights.x + evaluatedTriangle.V1.Color * evaluatedTriangle.BarycentricWeights.y
	    + evaluatedTriangle.V2.Color * evaluatedTriangle.BarycentricWeights.z;
}

void BuildRayTracingShadingFrame(RayTracingEvaluatedTriangle evaluatedTriangle,
                                 MeshInstanceData mesh,
                                 float3 outwardGeometricNormal,
                                 out float3 vertexNormal,
                                 out float3 tangentWorld,
                                 out float3 bitangentWorld)
{
	const float3 localNormal = evaluatedTriangle.V0.Normal * evaluatedTriangle.BarycentricWeights.x + evaluatedTriangle.V1.Normal * evaluatedTriangle.BarycentricWeights.y
	    + evaluatedTriangle.V2.Normal * evaluatedTriangle.BarycentricWeights.z;

	const float3 localTangent = evaluatedTriangle.V0.Tangent * evaluatedTriangle.BarycentricWeights.x + evaluatedTriangle.V1.Tangent * evaluatedTriangle.BarycentricWeights.y
	    + evaluatedTriangle.V2.Tangent * evaluatedTriangle.BarycentricWeights.z;

	const float interpolatedTangentSign = evaluatedTriangle.V0.TangentSign * evaluatedTriangle.BarycentricWeights.x + evaluatedTriangle.V1.TangentSign * evaluatedTriangle.BarycentricWeights.y
	    + evaluatedTriangle.V2.TangentSign * evaluatedTriangle.BarycentricWeights.z;

	const float tangentSign = interpolatedTangentSign >= 0.0f ? 1.0f : -1.0f;

	vertexNormal = normalize(mul(localNormal, (float3x3)mesh.WorldInverseTranspose));
	tangentWorld = normalize(mul(localTangent, (float3x3)mesh.WorldMatrix));

	if (dot(vertexNormal, outwardGeometricNormal) < 0.0f)
	{
		vertexNormal = -vertexNormal;
	}

	tangentWorld = OrthonormalizeTangent(tangentWorld, vertexNormal);
	bitangentWorld = ComputeBitangentFromSign(vertexNormal, tangentWorld, tangentSign);
}

#endif
