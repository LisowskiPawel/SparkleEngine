#pragma once

struct RayTracingHitSurfaceData
{
	bool bValid;
	float3 PositionWorld;
	float3 PreviousPositionWorld;
	float3 GeometricNormalWorld;
	float PositionError;
	float3 NormalWorld;
	float3 TangentWorld;
	float3 BaseColor;
	float3 EmissiveColor;
	float3 SubsurfaceColor;
	float Roughness;
	float Metallic;
	float DielectricF0;
	float AmbientOcclusion;
	float Alpha;
	float SubsurfaceStrength;
	uint AlphaMode;
	uint GpuSceneSlot;
	uint InstanceId;
	uint PrimitiveIndex;
	bool bEmissionTwoSided;
};
