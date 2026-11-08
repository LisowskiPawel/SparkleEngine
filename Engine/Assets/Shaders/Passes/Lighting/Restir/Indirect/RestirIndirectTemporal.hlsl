#include "/Engine/RayTracing/RayTracingSceneTraceInline.hlsli"
#include "/Engine/Resources/ViewTemporalUniformData.hlsli"

#include "/Engine/Passes/GBuffer/MotionVector.hlsli"

RWTexture2D<float4> TemporalReservoirSampleTexture;
RWTexture2D<float4> TemporalReservoirWeightTexture;
Texture2D<float4> PreviousReservoirSampleTexture;
Texture2D<float4> PreviousReservoirWeightTexture;
Texture2D<float4> PreviousReservoirSurfaceTexture;
Texture2D GBufferMotionVector;
Texture2D SkyTexture;
SamplerState SamplerLinearNoMipWrapClampClamp;

#include "/Engine/Passes/Lighting/Restir/Indirect/GBufferPathSurface.hlsli"
#include "/Engine/Resources/MeshInstanceShaderData.hlsli"

RaytracingAccelerationStructure SceneTlas;

#include "/Engine/Passes/Lighting/Restir/Indirect/RestirIndirectReservoirReuse.hlsli"

[numthreads(8, 8, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
	uint width = 0u;
	uint height = 0u;
	TemporalReservoirSampleTexture.GetDimensions(width, height);
	const uint2 pixelCoord = dispatchThreadId.xy;
	if (pixelCoord.x >= width || pixelCoord.y >= height)
	{
		return;
	}

	const GBufferPathSurface surface = LoadGBufferPathSurface(pixelCoord);
	IndirectReservoir::Reservoir reservoir = IndirectReservoir::BuildInitialReservoir(SceneTlas, surface, pixelCoord, SkyTexture, SamplerLinearNoMipWrapClampClamp);

	if (surface.bValid && RestirIndirectTemporalReuse != 0u && HistoryValid != 0u)
	{
		IndirectReservoir::ReuseTemporal(SceneTlas,
		                                 pixelCoord,
		                                 uint2(width, height),
		                                 surface,
		                                 GBufferMotionVector,
		                                 PreviousReservoirSampleTexture,
		                                 PreviousReservoirWeightTexture,
		                                 PreviousReservoirSurfaceTexture,
		                                 SkyTexture,
		                                 SamplerLinearNoMipWrapClampClamp,
		                                 reservoir);
	}

	TemporalReservoirSampleTexture[pixelCoord] = IndirectReservoir::PackSample(reservoir);
	TemporalReservoirWeightTexture[pixelCoord] = IndirectReservoir::PackWeight(reservoir);
}
