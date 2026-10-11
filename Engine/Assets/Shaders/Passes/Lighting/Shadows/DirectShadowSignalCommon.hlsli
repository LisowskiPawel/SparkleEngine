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

RayTracedShadowPreparation PrepareDirectShadowSignal(uint2 pixelCoord)
{
	RayTracedShadowPreparation preparation = (RayTracedShadowPreparation)0;
	preparation.ImmediateSignal = RayTracedShadowSignals::BuildUnshadowedSignal(0.0f);

	const float sceneDepth = LoadSceneDepth(pixelCoord);
	if (IsSkyPixel(sceneDepth))
	{
		return preparation;
	}

	const float3 positionWorld = ReconstructGBufferWorldPosition(pixelCoord, sceneDepth, InvViewMTX, InvProjectionMTX);
	const float3 normalWorld = GBufferPacking::DecodeWorldNormal(GBufferWorldNormal.Load(int3(pixelCoord, 0)).xyz);
	const DirectLightReservoir::Reservoir reservoir = DirectLightReservoir::UnpackReservoir(CurrentReservoirSample.Load(int3(pixelCoord, 0)), CurrentReservoirWeight.Load(int3(pixelCoord, 0)));
	if (!DirectLightReservoir::IsValid(reservoir))
	{
		return preparation;
	}

	const LightSampling::DirectLightSample lightSample = DirectLightReservoir::ReplayLightSample(reservoir, positionWorld);
	return RayTracedShadows::PrepareDirectLightShadow(positionWorld, normalWorld, lightSample, DirectLightSampling::CastsShadow(reservoir.Candidate.Light));
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

	const RayTracedShadowPreparation preparation = PrepareDirectShadowSignal(pixelCoord);
	ShadowVisibilitySample signal = preparation.ImmediateSignal;
	if (preparation.TraceRequired)
	{
		signal = RayTracedShadows::TraceShadowRay(preparation.Request);
	}

	ShadowVisibilitySignal[pixelCoord] = RayTracedShadowSignalPacking::PackShadowSignal(signal);
}

#endif
