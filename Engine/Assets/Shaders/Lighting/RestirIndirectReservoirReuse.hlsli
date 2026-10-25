#ifndef SPARKLE_RESTIR_INDIRECT_RESERVOIR_REUSE_HLSLI
#define SPARKLE_RESTIR_INDIRECT_RESERVOIR_REUSE_HLSLI

#include "/Engine/Lighting/RestirIndirectReservoir.hlsli"
#include "/Engine/Passes/GBuffer/MotionVector.hlsli"

namespace IndirectReservoir
{
	void ReuseTemporal(uint2 pixelCoord,
	                   uint2 extent,
	                   Surface surface,
	                   Texture2D motionVectors,
	                   Texture2D<float4> previousSamples,
	                   Texture2D<float4> previousWeights,
	                   Texture2D<float4> previousSurfaceTexture,
	                   Texture2D skyTexture,
	                   SamplerState skySampler,
	                   inout Reservoir reservoir)
	{
		const float2 motionPixels = motionVectors.Load(int3(pixelCoord, 0)).xy;
		const float2 previousPixel = MotionVectors::ReprojectToPreviousPixelCenter(pixelCoord, motionPixels, float2(extent.x, extent.y));
		const int2 previousPixelCoord = int2(floor(previousPixel));
		if (previousPixelCoord.x >= 0 && previousPixelCoord.y >= 0 && previousPixelCoord.x < int(extent.x) && previousPixelCoord.y < int(extent.y)
		    && AreSurfacesCompatible(surface, previousSurfaceTexture.Load(int3(previousPixelCoord, 0))))
		{
			const Reservoir previous = UnpackReservoir(previousSamples.Load(int3(previousPixelCoord, 0)), previousWeights.Load(int3(previousPixelCoord, 0)));

			uint rng = RestirReservoirCommon::BuildSeed(pixelCoord, 0x7E4F0A1u);
			CombineReservoir(reservoir, previous, surface, skyTexture, skySampler, RestirReservoirCommon::MaxTemporalM, CommonRandom::Random01(rng));
		}
	}

	void ReuseSpatial(uint2 pixelCoord,
	                  uint2 extent,
	                  Surface surface,
	                  Texture2D<float4> temporalSamples,
	                  Texture2D<float4> temporalWeights,
	                  Texture2D skyTexture,
	                  SamplerState skySampler,
	                  inout Reservoir reservoir)
	{
		uint rng = RestirReservoirCommon::BuildSeed(pixelCoord, 0x5A71A1u);
		const uint offsetStart = uint(CommonRandom::Random01(rng) * 8.0f) & 7u;
		[unroll]
		for (uint sampleIndex = 0u; sampleIndex < RestirReservoirCommon::SpatialNeighborCount; ++sampleIndex)
		{
			const int2 neighborCoord = int2(pixelCoord) + RestirReservoirCommon::SpatialOffsets[(offsetStart + sampleIndex * 2u) & 7u];
			if (neighborCoord.x < 0 || neighborCoord.y < 0 || neighborCoord.x >= int(extent.x) || neighborCoord.y >= int(extent.y))
			{
				continue;
			}

			const Surface neighborSurface = LoadSurface(uint2(neighborCoord));
			if (!AreSurfacesCompatible(surface, PackSurface(neighborSurface)))
			{
				continue;
			}

			const Reservoir neighbor = UnpackReservoir(temporalSamples.Load(int3(neighborCoord, 0)), temporalWeights.Load(int3(neighborCoord, 0)));

			CombineReservoir(reservoir, neighbor, surface, skyTexture, skySampler, RestirReservoirCommon::MaxSpatialM, CommonRandom::Random01(rng));
		}
	}
}

#endif
