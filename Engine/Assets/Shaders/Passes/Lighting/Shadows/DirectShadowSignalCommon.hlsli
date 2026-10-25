#ifndef SPARKLE_DIRECT_SHADOW_SIGNAL_COMMON_HLSLI
#define SPARKLE_DIRECT_SHADOW_SIGNAL_COMMON_HLSLI

#include "/Engine/Resources/ViewCameraUniformData.hlsli"

#include "/Engine/Lighting/DirectLightReservoirData.hlsli"
#include "/Engine/Passes/GBuffer/GBufferUtils.hlsli"
#include "/Engine/RayTracing/Shadows/RayTracedShadowSignalPacking.hlsli"
#include "/Engine/RayTracing/Shadows/RayTracedShadowSemantics.hlsli"

RaytracingAccelerationStructure SceneTlas;

#include "/Engine/RayTracing/Shadows/RayTracedShadowTrace.hlsli"

RWTexture2D<float4> ShadowVisibilitySignal;
Texture2D<float4> CurrentReservoirSample;
Texture2D<float4> CurrentReservoirWeight;

bool PrepareDirectShadowSignal(uint2 pixelCoord, out RayTracedShadowRequest request, out ShadowVisibilitySample immediateSignal)
{
	request = (RayTracedShadowRequest)0;
	immediateSignal = BuildUnshadowedSignal(0.0f);

	const float sceneDepth = LoadSceneDepth(pixelCoord);
	if (IsSkyPixel(sceneDepth))
	{
		return false;
	}

	const float3 positionWorld = ReconstructGBufferWorldPosition(pixelCoord, sceneDepth, InvViewMTX, InvProjectionMTX);
	const float3 normalWorld = GBufferPacking::DecodeWorldNormal(GBufferWorldNormal.Load(int3(pixelCoord, 0)).xyz);
	const DirectReservoir::Reservoir reservoir = DirectReservoir::UnpackReservoir(CurrentReservoirSample.Load(int3(pixelCoord, 0)), CurrentReservoirWeight.Load(int3(pixelCoord, 0)));
	if (!DirectReservoir::IsValid(reservoir))
	{
		return false;
	}

	const LightSampling::DirectLightSample lightSample = DirectReservoir::ReplayLightSample(reservoir, positionWorld);
	return ShadowRays::PrepareDirectLightShadow(positionWorld, normalWorld, lightSample, DirectLightSampling::CastsShadow(reservoir.Candidate.Light), request, immediateSignal);
}

void TraceAndStoreDirectShadowSignal(uint2 pixelCoord)
{
	uint width = 0;
	uint height = 0;
	ShadowVisibilitySignal.GetDimensions(width, height);
	if (pixelCoord.x >= width || pixelCoord.y >= height)
	{
		return;
	}

	RayTracedShadowRequest request;
	ShadowVisibilitySample signal;
	if (PrepareDirectShadowSignal(pixelCoord, request, signal))
	{
		signal = ShadowRays::TraceShadowRay(request);
	}

	ShadowVisibilitySignal[pixelCoord] = PackShadowSignal(signal);
}

#endif
