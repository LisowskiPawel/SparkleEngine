#include "/Engine/Resources/ViewTemporalUniformData.hlsli"
#include "/Engine/Resources/MeshInstanceShaderData.hlsli"

#include "/Engine/Geometry/VertexInput.hlsli"
#include "/Engine/Geometry/VertexOutput.hlsli"
#include "/Engine/Geometry/Transforms.hlsli"
#include "/Engine/Geometry/Basis.hlsli"
#include "/Engine/Geometry/Morphing.hlsli"
#include "/Engine/Geometry/Skinning.hlsli"

void main(in VS::Input Input, out VS::Output Output)
{
	const uint packedInstanceId = FirstInstance + Input.InstanceId;
	const uint instanceSlot = MeshInstanceSlots[packedInstanceId];
	const MeshInstanceData meshInstance = MeshInstances[instanceSlot];
	const float4x4 worldMatrix = meshInstance.WorldMatrix;
	const float4x4 previousWorldMatrix = meshInstance.PreviousWorldMatrix;
	const float3x3 worldInverseTranspose = (float3x3)meshInstance.WorldInverseTranspose;

	const MorphedVertexAttributes morphedVertex = ApplyMorphing(meshInstance, 0u, Input.VertexId, Input.Position, Input.Normal, Input.Tangent.xyz, MorphWeights);
	const MorphedVertexAttributes previousMorphedVertex = ApplyMorphing(meshInstance, 0u, Input.VertexId, Input.Position, Input.Normal, Input.Tangent.xyz, PreviousMorphWeights);
	const SkinnedVertexAttributes localVertex = ApplySkinning(meshInstance, Input.VertexId, morphedVertex.Position, morphedVertex.Normal, morphedVertex.Tangent, JointMatrices);

	const SkinnedVertexAttributes previousLocalVertex = ApplySkinning(meshInstance,
	                                                                  Input.VertexId,
	                                                                  previousMorphedVertex.Position,
	                                                                  previousMorphedVertex.Normal,
	                                                                  previousMorphedVertex.Tangent,
	                                                                  PreviousJointMatrices);

	const float4 positionWorld = mul(float4(localVertex.Position, 1.0f), worldMatrix);
	const float4 previousPositionWorld = mul(float4(previousLocalVertex.Position, 1.0f), previousWorldMatrix);
	const float3 normalWorld = normalize(mul(localVertex.Normal, worldInverseTranspose));
	const float3 tangentWorld = OrthonormalizeTangent(mul(localVertex.Tangent, (float3x3)worldMatrix), normalWorld);

	const float4 positionClip = PositionWorldToClip(positionWorld);
	const float4 previousClipPosition = mul(previousPositionWorld, PreviousWorldToClipMatrix);
	const float4 jitteredPositionClip = ApplyTemporalJitterClipOffset(positionClip, CurrentJitterNdc);

	// Rasterize jittered samples for DLSS/DLAA, but keep motion-vector inputs unjittered.
	Output.Position = jitteredPositionClip;
	Output.NormalWorld = normalWorld;
	Output.TangentWorld = float4(tangentWorld, Input.Tangent.w);
	Output.TexCoord = Input.TexCoord;
	Output.PrevClipPosition = previousClipPosition;
	Output.GpuSceneSlot = meshInstance.GpuSceneSlot;
}
