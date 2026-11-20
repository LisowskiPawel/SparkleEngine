#ifndef SPARKLE_DIRECT_LIGHT_RESERVOIR_REUSE_HLSLI
#define SPARKLE_DIRECT_LIGHT_RESERVOIR_REUSE_HLSLI

#include "/Engine/Passes/Lighting/Direct/DirectLightReservoir.hlsli"
#include "/Engine/Passes/GBuffer/MotionVector.hlsli"

namespace DirectReservoir
{
	void ReuseTemporal(uint2 pixelCoord,
	                   uint2 extent,
	                   Surface surface,
	                   Texture2D motionVectors,
	                   Texture2D<float4> previousSamples,
	                   Texture2D<float4> previousWeights,
	                   Texture2D<float4> previousSurfaceTexture,
	                   inout Reservoir reservoir)
	{
		const float2 motionPixels = motionVectors.Load(int3(pixelCoord, 0)).xy;
		const float2 previousPixel = MotionVectors::ReprojectToPreviousPixelCenter(pixelCoord, motionPixels, float2(extent.x, extent.y));
		const int2 previousPixelCoord = int2(floor(previousPixel));
		const bool bPreviousXInBounds = previousPixelCoord.x >= 0 && previousPixelCoord.x < (int)extent.x;
		const bool bPreviousYInBounds = previousPixelCoord.y >= 0 && previousPixelCoord.y < (int)extent.y;
		if (bPreviousXInBounds && bPreviousYInBounds)
		{
			const float4 previousSurface = previousSurfaceTexture.Load(int3(previousPixelCoord, 0));
			if (AreSurfacesCompatible(surface, previousSurface))
			{
				const float4 samplePayload = previousSamples.Load(int3(previousPixelCoord, 0));
				const float4 weightPayload = previousWeights.Load(int3(previousPixelCoord, 0));
				const Reservoir previousReservoir = UnpackReservoir(samplePayload, weightPayload);

				uint rng = RestirReservoirCommon::BuildSeed(pixelCoord, 0x7151u);
				const float selectionSample = CommonRandom::Random01(rng);
				CombineReservoir(reservoir, previousReservoir, surface, RestirReservoirCommon::MaxTemporalM, selectionSample);
			}
		}
	}

	void ReuseSpatial(uint2 pixelCoord, uint2 extent, Surface surface, Texture2D<float4> temporalSamples, Texture2D<float4> temporalWeights, inout Reservoir reservoir)
	{
		uint rng = RestirReservoirCommon::BuildSeed(pixelCoord, 0x5A71A1u);
		const uint offsetStart = (uint)(CommonRandom::Random01(rng) * 8.0f) & 7u;
		[unroll]
		for (uint sampleIndex = 0u; sampleIndex < RestirReservoirCommon::SpatialNeighborCount; ++sampleIndex)
		{
			const int2 neighborCoord = int2(pixelCoord) + RestirReservoirCommon::SpatialOffsets[(offsetStart + sampleIndex * 2u) & 7u];
			const bool bNeighborXInBounds = neighborCoord.x >= 0 && neighborCoord.x < (int)extent.x;
			const bool bNeighborYInBounds = neighborCoord.y >= 0 && neighborCoord.y < (int)extent.y;
			if (!bNeighborXInBounds || !bNeighborYInBounds)
			{
				continue;
			}

			const Surface neighborSurface = LoadSurface((uint2)neighborCoord);
			const float4 packedNeighborSurface = PackSurface(neighborSurface);
			if (!AreSurfacesCompatible(surface, packedNeighborSurface))
			{
				continue;
			}

			const float4 samplePayload = temporalSamples.Load(int3(neighborCoord, 0));
			const float4 weightPayload = temporalWeights.Load(int3(neighborCoord, 0));
			const Reservoir neighborReservoir = UnpackReservoir(samplePayload, weightPayload);

			const float selectionSample = CommonRandom::Random01(rng);
			CombineReservoir(reservoir, neighborReservoir, surface, RestirReservoirCommon::MaxSpatialM, selectionSample);
		}
	}
}

#endif
