#include "/Engine/RayTracing/RayTracingSceneTraceInline.hlsli"

Texture2D<float4> TemporalReservoirSampleTexture;
Texture2D<float4> TemporalReservoirWeightTexture;
RWTexture2D<float4> CurrentReservoirSampleTexture;
RWTexture2D<float4> CurrentReservoirWeightTexture;
RWTexture2D<float4> CurrentReservoirSurfaceTexture;
Texture2D SkyTexture;
SamplerState SamplerLinearNoMipWrapClampClamp;

#include "/Engine/Resources/FrameUniformData.hlsli"
#include "/Engine/Passes/Lighting/Restir/Indirect/GBufferPathSurface.hlsli"
#include "/Engine/Resources/MeshInstanceShaderData.hlsli"

RaytracingAccelerationStructure SceneTlas;

#include "/Engine/Passes/Lighting/Restir/Indirect/RestirIndirectReservoirReuse.hlsli"

[numthreads(8, 8, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
	uint width = 0u;
	uint height = 0u;
	CurrentReservoirSampleTexture.GetDimensions(width, height);
	const uint2 pixelCoord = dispatchThreadId.xy;
	if (pixelCoord.x >= width || pixelCoord.y >= height)
	{
		return;
	}

	const GBufferPathSurface surface = LoadGBufferPathSurface(pixelCoord);

	IndirectReservoir::Reservoir reservoir = IndirectReservoir::UnpackReservoir(TemporalReservoirSampleTexture.Load(int3(pixelCoord, 0)), TemporalReservoirWeightTexture.Load(int3(pixelCoord, 0)));

	if (surface.bValid && RestirIndirectSpatialReuse != 0u)
	{
		IndirectReservoir::ReuseSpatial(SceneTlas,
		                                pixelCoord,
		                                uint2(width, height),
		                                surface,
		                                TemporalReservoirSampleTexture,
		                                TemporalReservoirWeightTexture,
		                                SkyTexture,
		                                SamplerLinearNoMipWrapClampClamp,
		                                reservoir);
	}

	CurrentReservoirSampleTexture[pixelCoord] = IndirectReservoir::PackSample(reservoir);
	CurrentReservoirWeightTexture[pixelCoord] = IndirectReservoir::PackWeight(reservoir);
	CurrentReservoirSurfaceTexture[pixelCoord] = IndirectReservoir::PackSurface(surface);
}
