#include "/Engine/Resources/ViewTemporalUniformData.hlsli"

#include "/Engine/Passes/GBuffer/MotionVector.hlsli"

RWTexture2D<float4> TemporalReservoirSampleTexture;
RWTexture2D<float4> TemporalReservoirWeightTexture;
Texture2D<float4> PreviousReservoirSampleTexture;
Texture2D<float4> PreviousReservoirWeightTexture;
Texture2D<float4> PreviousReservoirSurfaceTexture;
Texture2D GBufferMotionVector;
Texture2D SkyTexture;
SamplerState SamplerLinearClamp;

#include "/Engine/Lighting/RestirIndirectReservoir.hlsli"

void ReuseTemporalReservoir(uint2 pixelCoord, uint2 extent, RestirIndirectReservoir::Surface surface, inout RestirIndirectReservoir::Reservoir reservoir)
{
	if (surface.Valid && RestirIndirectTemporalReuse != 0u && HistoryValid != 0u)
	{
		const float2 motionPixels = GBufferMotionVector.Load(int3(pixelCoord, 0)).xy;
		const float2 previousPixel = MotionVectors::ReprojectToPreviousPixelCenter(pixelCoord, motionPixels, float2(extent.x, extent.y));
		const int2 previousPixelCoord = int2(floor(previousPixel));
		if (previousPixelCoord.x >= 0 && previousPixelCoord.y >= 0 && previousPixelCoord.x < int(extent.x) && previousPixelCoord.y < int(extent.y)
		    && RestirIndirectReservoir::AreSurfacesCompatible(surface, PreviousReservoirSurfaceTexture.Load(int3(previousPixelCoord, 0))))
		{
			const RestirIndirectReservoir::Reservoir previous = RestirIndirectReservoir::UnpackReservoir(PreviousReservoirSampleTexture.Load(int3(previousPixelCoord, 0)),
			                                                                                             PreviousReservoirWeightTexture.Load(int3(previousPixelCoord, 0)));

			uint rng = RestirReservoirCommon::BuildSeed(pixelCoord, 0x7E4F0A1u);
			RestirIndirectReservoir::CombineReservoir(reservoir, previous, surface, SkyTexture, SamplerLinearClamp, RestirReservoirCommon::MaxTemporalM, CommonRandom::Random01(rng));
		}
	}
}

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

	const RestirIndirectReservoir::Surface surface = RestirIndirectReservoir::LoadSurface(pixelCoord);
	RestirIndirectReservoir::Reservoir reservoir = RestirIndirectReservoir::BuildInitialReservoir(surface, pixelCoord, SkyTexture, SamplerLinearClamp);

	ReuseTemporalReservoir(pixelCoord, uint2(width, height), surface, reservoir);

	TemporalReservoirSampleTexture[pixelCoord] = RestirIndirectReservoir::PackSample(reservoir);
	TemporalReservoirWeightTexture[pixelCoord] = RestirIndirectReservoir::PackWeight(reservoir);
}
