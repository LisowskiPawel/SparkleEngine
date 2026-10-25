#pragma once

#include "/Engine/Geometry/Basis.hlsli"
#include "/Engine/RayTracing/RayEndpoints.hlsli"
#include "/Engine/RayTracing/RayTracingHitSurface.hlsli"
#include "/Engine/RayTracing/RayTracingMaterialSampling.hlsli"
#include "/Engine/RayTracing/RayTracingTraceResult.hlsli"

RayTracingHitSurfaceData ReconstructRayTracingHitSurface(RayTracingTraceResult trace)
{
	const RayTracingEvaluatedTriangle evaluatedTriangle = EvaluateRayTracingTriangle(trace.InstanceId, trace.PrimitiveIndex, trace.Barycentrics);
	const MeshInstanceData mesh = MeshInstances[trace.InstanceId];
	const float3 positionObject = InterpolateRayTracingPosition(evaluatedTriangle);
	const float3 positionWorld = RayEndpoints::TransformPosition(positionObject, mesh.WorldMatrix);
	const float3 normalObject = cross(evaluatedTriangle.V1.Position - evaluatedTriangle.V0.Position, evaluatedTriangle.V2.Position - evaluatedTriangle.V0.Position);
	const float3 outwardGeometricNormal = RayEndpoints::TransformGeometricNormal(normalObject, mesh);

	const bool twoSided = (evaluatedTriangle.Instance.Flags & HitSurface::InstanceFlagTwoSided) != 0u;

	RayTracingHitSurfaceData surface = (RayTracingHitSurfaceData)0;
	if (!trace.FrontFace && !twoSided)
	{
		return surface;
	}

	const float faceSign = trace.FrontFace ? 1.0f : -1.0f;
	const float3 geometricNormal = outwardGeometricNormal * faceSign;

	float3 vertexNormal;
	float3 tangentWorld;
	float3 bitangentWorld;
	BuildRayTracingShadingFrame(evaluatedTriangle, mesh, outwardGeometricNormal, vertexNormal, tangentWorld, bitangentWorld);

	const float2 texCoord0 = InterpolateRayTracingTexCoord0(evaluatedTriangle);
	const RayTracingMaterialSample material = SampleRayTracingMaterial(evaluatedTriangle.Material, texCoord0, InterpolateRayTracingColor(evaluatedTriangle), EmissiveEnabled != 0u);

	float3 shadingNormal = TransformTangentNormalToWorld(material.NormalTangent, vertexNormal, tangentWorld, bitangentWorld);
	shadingNormal *= faceSign;
	tangentWorld *= faceSign;

	if (dot(shadingNormal, geometricNormal) <= 0.0f)
	{
		return surface;
	}

	surface.Valid = true;
	surface.PositionWorld = positionWorld;
	surface.PreviousPositionWorld = positionWorld;

	surface.GeometricNormalWorld = geometricNormal;
	surface.PositionError = RayEndpoints::SurfaceErrorBound(evaluatedTriangle, mesh, positionObject, positionWorld, normalObject);
	surface.NormalWorld = shadingNormal;
	surface.TangentWorld = tangentWorld;

	surface.BaseColor = material.BaseColor.rgb;
	surface.EmissiveColor = material.Emissive;
	surface.SubsurfaceColor = material.SubsurfaceColor;
	surface.Roughness = material.Roughness;
	surface.Metallic = material.Metallic;
	surface.DielectricF0 = evaluatedTriangle.Material.F0;
	surface.AmbientOcclusion = material.AmbientOcclusion;
	surface.Alpha = material.BaseColor.a;
	surface.SubsurfaceStrength = material.SubsurfaceStrength;
	surface.AlphaMode = evaluatedTriangle.Material.AlphaMode;

	surface.GpuSceneSlot = mesh.GpuSceneSlot;
	surface.InstanceId = trace.InstanceId;
	surface.PrimitiveIndex = trace.PrimitiveIndex;
	surface.EmissionTwoSided = twoSided;

	return surface;
}

RayTracingHitSurfaceData ReconstructRayTracingHitSurfaceWithPrevious(RayTracingTraceResult trace)
{
	RayTracingHitSurfaceData surface = ReconstructRayTracingHitSurface(trace);
	if (surface.Valid)
	{
		const RayTracingHitTriangle hitTriangle = LoadRayTracingHitTriangle(trace.InstanceId, trace.PrimitiveIndex, trace.Barycentrics);
		const MeshInstanceData mesh = MeshInstances[trace.InstanceId];
		const float3 previousPositionObject = EvaluatePreviousRayTracingPosition(hitTriangle, mesh);

		surface.PreviousPositionWorld = RayEndpoints::TransformPosition(previousPositionObject, mesh.PreviousWorldMatrix);
	}

	return surface;
}
