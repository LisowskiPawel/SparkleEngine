#include "/Engine/Resources/ViewCameraUniformData.hlsli"

#include "/Engine/Lighting/DirectLightReservoir.hlsli"
#include "/Engine/Lighting/SurfaceLighting.hlsli"
#include "/Engine/Passes/GBuffer/GBufferUtils.hlsli"
#include "/Engine/RayTracing/Shadows/RayTracedShadowSignalPacking.hlsli"

RWTexture2D<float4> DirectDiffuse;
RWTexture2D<float4> DirectSpecular;
RWTexture2D<float4> DirectSubsurface;
Texture2D<float4> ShadowVisibilitySignal;
Texture2D<float4> CurrentReservoirSample;
Texture2D<float4> CurrentReservoirWeight;

void AddDirectLightSample(GBufferData gBuffer,
                          float3 viewDirWorld,
                          bool evaluateSubsurface,
                          LightSampling::DirectLightSample lightSample,
                          float visibility,
                          float sampleWeight,
                          inout SurfaceLighting::Contribution directLighting)
{
	if (!lightSample.Valid || sampleWeight <= 0.0f)
	{
		return;
	}

	SurfaceLighting::Surface lightingSurface = {viewDirWorld,
	                                            gBuffer.NormalWorld,
	                                            gBuffer.BaseColor,
	                                            gBuffer.Roughness,
	                                            gBuffer.Metallic,
	                                            gBuffer.DielectricF0,
	                                            gBuffer.SubsurfaceColor,
	                                            gBuffer.SubsurfaceStrength,
	                                            DirectLightingEvaluateDiffuse != 0u,
	                                            DirectLightingEvaluateSpecular != 0u,
	                                            evaluateSubsurface};

	const SurfaceLighting::Contribution contribution = SurfaceLighting::EvaluateDirectLightSample(lightingSurface, lightSample, visibility);

	directLighting.Diffuse += contribution.Diffuse * sampleWeight;
	directLighting.Specular += contribution.Specular * sampleWeight;
	directLighting.Subsurface += contribution.Subsurface * sampleWeight;
}

SurfaceLighting::Contribution EvaluateDirectLighting(uint2 pixelCoord, GBufferData gBuffer)
{
	const float3 positionWorld = ReconstructGBufferWorldPosition(pixelCoord, gBuffer.SceneDepth, InvViewMTX, InvProjectionMTX);
	const float3 viewDirWorld = normalize(Position - positionWorld);

	SurfaceLighting::Contribution directLighting = (SurfaceLighting::Contribution)0;

	const DirectLightReservoir::Reservoir reservoir = DirectLightReservoir::UnpackReservoir(CurrentReservoirSample.Load(int3(pixelCoord, 0)), CurrentReservoirWeight.Load(int3(pixelCoord, 0)));

	float visibility = 1.0f;
	if (DirectLightingEvaluateShadows != 0u)
	{
		visibility = RayTracedShadowSignalPacking::UnpackShadowSignal(ShadowVisibilitySignal.Load(int3(pixelCoord, 0))).Visibility;
	}

	const bool evaluateSubsurface = DirectLightingEvaluateSubsurface != 0u && HasSubsurface(gBuffer);
	if (DirectLightReservoir::IsValid(reservoir))
	{
		const LightSampling::DirectLightSample lightSample = DirectLightReservoir::ReplayLightSample(reservoir, positionWorld);
		const float reservoirWeight = DirectLightReservoir::GetFinalWeight(reservoir);

		AddDirectLightSample(gBuffer, viewDirWorld, evaluateSubsurface, lightSample, visibility, reservoirWeight, directLighting);
	}

	return directLighting;
}

void StoreDirectLighting(uint2 pixelCoord, SurfaceLighting::Contribution directLighting, float alpha)
{
	if (DirectLightingEvaluateDiffuse != 0u)
	{
		DirectDiffuse[pixelCoord] = float4(directLighting.Diffuse, alpha);
	}
	if (DirectLightingEvaluateSpecular != 0u)
	{
		DirectSpecular[pixelCoord] = float4(directLighting.Specular, alpha);
	}
	if (DirectLightingEvaluateSubsurface != 0u)
	{
		DirectSubsurface[pixelCoord] = float4(directLighting.Subsurface, alpha);
	}
}

[numthreads(8, 8, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
	uint width = 0;
	uint height = 0;
	DirectDiffuse.GetDimensions(width, height);
	const uint2 pixelCoord = dispatchThreadId.xy;
	if (pixelCoord.x >= width || pixelCoord.y >= height)
	{
		return;
	}

	const GBufferData gBuffer = LoadGBuffer(pixelCoord);
	if (IsSkyPixel(gBuffer.SceneDepth))
	{
		StoreDirectLighting(pixelCoord, (SurfaceLighting::Contribution)0, 0.0f);
		return;
	}

	const SurfaceLighting::Contribution directLighting = EvaluateDirectLighting(pixelCoord, gBuffer);
	StoreDirectLighting(pixelCoord, directLighting, gBuffer.Alpha);
}
