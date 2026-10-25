#include "/Engine/Lighting/DirectLightReservoir.hlsli"
#include "/Engine/RayTracing/Shadows/RayTracedShadowSignalPacking.hlsli"

RWTexture2D<float4> DirectDiffuse;
RWTexture2D<float4> DirectSpecular;
RWTexture2D<float4> DirectSubsurface;
Texture2D<float4> ShadowVisibilitySignal;
Texture2D<float4> CurrentReservoirSample;
Texture2D<float4> CurrentReservoirWeight;

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

	const DirectReservoir::Reservoir reservoir = DirectReservoir::UnpackReservoir(CurrentReservoirSample.Load(int3(pixelCoord, 0)), CurrentReservoirWeight.Load(int3(pixelCoord, 0)));

	float visibility = 1.0f;
	if (DirectLightingEvaluateShadows != 0u)
	{
		visibility = UnpackShadowSignal(ShadowVisibilitySignal.Load(int3(pixelCoord, 0))).Visibility;
	}

	const SurfaceLighting::Contribution directLighting = DirectReservoir::EvaluateSelectedLighting(pixelCoord, gBuffer, reservoir, visibility);
	StoreDirectLighting(pixelCoord, directLighting, gBuffer.Alpha);
}
