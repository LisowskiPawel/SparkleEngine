#ifndef SPARKLE_RAY_TRACING_GBUFFER_COMMON_HLSLI
#define SPARKLE_RAY_TRACING_GBUFFER_COMMON_HLSLI

#include "/Engine/Resources/ViewUniformData.hlsli"
#include "/Engine/Resources/ViewCameraUniformData.hlsli"
#include "/Engine/Resources/ViewTemporalUniformData.hlsli"

#include "/Engine/Geometry/ScreenSpace.hlsli"
#include "/Engine/Geometry/Transforms.hlsli"
#include "/Engine/Passes/GBuffer/GBufferPacking.hlsli"
#include "/Engine/Passes/GBuffer/MotionVector.hlsli"

#include "/Engine/RayTracing/RayTracingMaterialHit.hlsli"
#include "/Engine/RayTracing/RayTracingSceneTrace.hlsli"

RWTexture2D<float4> GBufferBaseColor;
RWTexture2D<float4> GBufferWorldNormal;
RWTexture2D<float4> GBufferWorldTangent;
RWTexture2D<float4> GBufferMaterial;
RWTexture2D<float4> GBufferEmissive;
RWTexture2D<float4> GBufferSubsurface;
RWTexture2D<float> GBufferDeviceZ;
RWTexture2D<float2> GBufferMotionVector;
RWTexture2D<uint> GBufferInstanceIdentity;
RaytracingAccelerationStructure SceneTlas;

namespace RayTracingGBuffer
{
	static const uint CullFlags = RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
	static const uint InstanceMask = 0xFFu;
	static const float MinimumT = 0.001f;

	RayDesc BuildPrimaryRay(uint2 pixelCoord)
	{
		RayDesc ray;
		ray.Origin = Position;
		ray.Direction = ComputeSkyViewDirectionWorld(pixelCoord);
		ray.TMin = MinimumT;
		ray.TMax = FarZ;
		return ray;
	}

	float ComputeDeviceZ(float3 positionWorld)
	{
		const float4 positionClip = PositionWorldToClip(float4(positionWorld, 1.0f));
		return positionClip.z / max(positionClip.w, 1.0e-6f);
	}

	float2 ComputeMotionVector(RayTracingHitSurfaceData surface)
	{
		const float4 currentClipPosition = PositionWorldToClip(float4(surface.PositionWorld, 1.0f));
		const float4 previousClipPosition = mul(float4(surface.PreviousPositionWorld, 1.0f), PreviousWorldToClipMatrix);
		return MotionVectors::Compute(currentClipPosition, previousClipPosition, ViewportSize);
	}

	void StoreMiss(uint2 pixelCoord)
	{
		GBufferBaseColor[pixelCoord] = GBufferPacking::PackSkyBaseColor();
		GBufferWorldNormal[pixelCoord] = GBufferPacking::PackSkyWorldNormal();
		GBufferWorldTangent[pixelCoord] = GBufferPacking::PackSkyWorldTangent();
		GBufferMaterial[pixelCoord] = GBufferPacking::PackSkyMaterial();
		GBufferEmissive[pixelCoord] = GBufferPacking::PackSkyEmissive();
		GBufferSubsurface[pixelCoord] = GBufferPacking::PackSkySubsurface();
		GBufferDeviceZ[pixelCoord] = GBufferPacking::PackSkyDeviceZ();
		GBufferMotionVector[pixelCoord] = 0.0f.xx;
		GBufferInstanceIdentity[pixelCoord] = 0u;
	}

	void StoreHit(uint2 pixelCoord, RayTracingHitSurfaceData surface)
	{
		GBufferBaseColor[pixelCoord] = GBufferPacking::PackBaseColor(surface.BaseColor, surface.Alpha, surface.AlphaMode, HitSurface::AlphaModeBlended);
		GBufferWorldNormal[pixelCoord] = GBufferPacking::PackWorldNormal(surface.NormalWorld);
		GBufferWorldTangent[pixelCoord] = GBufferPacking::PackWorldTangent(surface.TangentWorld);
		GBufferMaterial[pixelCoord] = GBufferPacking::PackMaterial(surface.Metallic, surface.Roughness, surface.AmbientOcclusion, surface.DielectricF0);
		GBufferEmissive[pixelCoord] = GBufferPacking::PackEmissive(surface.EmissiveColor);
		GBufferSubsurface[pixelCoord] = GBufferPacking::PackSubsurface(surface.SubsurfaceColor, surface.SubsurfaceStrength);
		GBufferDeviceZ[pixelCoord] = ComputeDeviceZ(surface.PositionWorld);
		GBufferMotionVector[pixelCoord] = ComputeMotionVector(surface);
		GBufferInstanceIdentity[pixelCoord] = GBufferWriteInstanceIdentity != 0u ? surface.GpuSceneSlot + 1u : 0u;
	}

	void StoreTraceResult(uint2 pixelCoord, RayTracingTraceResult trace)
	{
		if (!trace.Hit)
		{
			StoreMiss(pixelCoord);
			return;
		}

		const RayTracingHitSurfaceData surface = ReconstructRayTracingHitSurfaceWithPrevious(trace);
		if (!surface.Valid)
		{
			StoreMiss(pixelCoord);
			return;
		}

		StoreHit(pixelCoord, surface);
	}

	void TraceAndStore(uint2 pixelCoord)
	{
		const RayDesc ray = BuildPrimaryRay(pixelCoord);
		const RayTracingTraceResult trace = TraceSceneRay(SceneTlas, ray.Origin, ray.Direction, ray.TMin, ray.TMax, CullFlags, InstanceMask);
		StoreTraceResult(pixelCoord, trace);
	}
}

#endif
