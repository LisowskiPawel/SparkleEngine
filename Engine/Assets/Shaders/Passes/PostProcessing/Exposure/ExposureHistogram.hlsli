#ifndef SPARKLE_EXPOSURE_HISTOGRAM_HLSLI
#define SPARKLE_EXPOSURE_HISTOGRAM_HLSLI

#include "/Engine/Display/Exposure.hlsli"

namespace ExposureHistogram
{
	static const uint BinCount = 512u;
	static const float MinimumLogLuminance = -16.0f;
	static const float MaximumLogLuminance = 16.0f;
	static const float LowPercentile = 0.1f;
	static const float HighPercentile = 0.9f;

	uint FindBin(float luminance)
	{
		if (luminance <= 0.0f)
		{
			return 0u;
		}

		const float normalized = saturate((log2(luminance) - MinimumLogLuminance) / (MaximumLogLuminance - MinimumLogLuminance));
		return 1u + min(uint(normalized * float(BinCount - 1u)), BinCount - 2u);
	}

	float BinLuminance(uint bin)
	{
		if (bin == 0u)
		{
			return 0.0f;
		}

		const float normalized = (float(bin - 1u) + 0.5f) / float(BinCount - 1u);
		return exp2(lerp(MinimumLogLuminance, MaximumLogLuminance, normalized));
	}

	uint CountSamples(StructuredBuffer<uint> histogramCounts)
	{
		uint sampleCount = 0u;
		[loop]
		for (uint bin = 0u; bin < BinCount; ++bin)
		{
			sampleCount += histogramCounts[bin];
		}

		return sampleCount;
	}

	float ResolveAverageLuminance(StructuredBuffer<uint> histogramCounts, uint sampleCount)
	{
		const float lowCount = float(sampleCount) * LowPercentile;
		const float highCount = float(sampleCount) * HighPercentile;
		float cumulativeCount = 0.0f;
		float weightedLuminance = 0.0f;
		float retainedCount = 0.0f;
		[loop]
		for (uint bin = 0u; bin < BinCount; ++bin)
		{
			const float nextCount = cumulativeCount + float(histogramCounts[bin]);
			const float weight = max(min(nextCount, highCount) - max(cumulativeCount, lowCount), 0.0f);
			weightedLuminance += BinLuminance(bin) * weight;
			retainedCount += weight;
			cumulativeCount = nextCount;
		}

		const float averageLuminance = retainedCount > 0.0f ? max(weightedLuminance / retainedCount, Exposure::MinimumMeteredLuminance) : Exposure::MinimumMeteredLuminance;
		return averageLuminance;
	}
}

#endif
