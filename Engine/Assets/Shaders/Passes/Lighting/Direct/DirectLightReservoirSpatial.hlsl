#include "/Engine/Passes/Lighting/Direct/DirectLightReservoirReuse.hlsli"

Texture2D<float4> TemporalReservoirSample;
Texture2D<float4> TemporalReservoirWeight;
RWTexture2D<float4> CurrentReservoirSample;
RWTexture2D<float4> CurrentReservoirWeight;
RWTexture2D<float4> CurrentReservoirSurface;

[numthreads(8, 8, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
	uint width = 0;
	uint height = 0;
	CurrentReservoirSample.GetDimensions(width, height);

	const uint2 pixelCoord = dispatchThreadId.xy;
	if (pixelCoord.x >= width || pixelCoord.y >= height)
	{
		return;
	}

	const DirectReservoir::Surface surface = DirectReservoir::LoadSurface(pixelCoord);
	if (!surface.bValid)
	{
		const DirectReservoir::Reservoir emptyReservoir = DirectReservoir::EmptyReservoir();
		CurrentReservoirSample[pixelCoord] = DirectReservoir::PackReservoirSample(emptyReservoir);
		CurrentReservoirWeight[pixelCoord] = DirectReservoir::PackReservoirWeight(emptyReservoir);
		CurrentReservoirSurface[pixelCoord] = 0.0f.xxxx;
		return;
	}

	const float4 samplePayload = TemporalReservoirSample.Load(int3(pixelCoord, 0));
	const float4 weightPayload = TemporalReservoirWeight.Load(int3(pixelCoord, 0));
	DirectReservoir::Reservoir reservoir = DirectReservoir::UnpackReservoir(samplePayload, weightPayload);

	DirectReservoir::ReuseSpatial(pixelCoord, uint2(width, height), surface, TemporalReservoirSample, TemporalReservoirWeight, reservoir);

	CurrentReservoirSample[pixelCoord] = DirectReservoir::PackReservoirSample(reservoir);
	CurrentReservoirWeight[pixelCoord] = DirectReservoir::PackReservoirWeight(reservoir);
	CurrentReservoirSurface[pixelCoord] = DirectReservoir::PackSurface(surface);
}
