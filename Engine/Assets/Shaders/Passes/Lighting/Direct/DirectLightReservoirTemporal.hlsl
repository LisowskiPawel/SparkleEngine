#include "/Engine/Resources/ViewTemporalUniformData.hlsli"

#include "/Engine/Lighting/DirectLightReservoirReuse.hlsli"

RWTexture2D<float4> TemporalReservoirSample;
RWTexture2D<float4> TemporalReservoirWeight;
Texture2D<float4> PreviousReservoirSample;
Texture2D<float4> PreviousReservoirWeight;
Texture2D<float4> PreviousReservoirSurface;
Texture2D GBufferMotionVector;

[numthreads(8, 8, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
	uint width = 0;
	uint height = 0;
	TemporalReservoirSample.GetDimensions(width, height);

	const uint2 pixelCoord = dispatchThreadId.xy;
	if (pixelCoord.x >= width || pixelCoord.y >= height)
	{
		return;
	}

	const DirectReservoir::Surface surface = DirectReservoir::LoadSurface(pixelCoord);
	if (!surface.Valid)
	{
		TemporalReservoirSample[pixelCoord] = DirectReservoir::PackReservoirSample(DirectReservoir::EmptyReservoir());
		TemporalReservoirWeight[pixelCoord] = DirectReservoir::PackReservoirWeight(DirectReservoir::EmptyReservoir());
		return;
	}

	DirectReservoir::Reservoir reservoir = DirectReservoir::BuildInitialReservoir(surface, pixelCoord);

	if (HistoryValid != 0u)
	{
		DirectReservoir::ReuseTemporal(pixelCoord, uint2(width, height), surface, GBufferMotionVector, PreviousReservoirSample, PreviousReservoirWeight, PreviousReservoirSurface, reservoir);
	}

	TemporalReservoirSample[pixelCoord] = DirectReservoir::PackReservoirSample(reservoir);
	TemporalReservoirWeight[pixelCoord] = DirectReservoir::PackReservoirWeight(reservoir);
}
