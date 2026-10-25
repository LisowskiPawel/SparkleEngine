#include "/Engine/Passes/PostProcessing/Exposure/ExposureHistogram.hlsli"

StructuredBuffer<uint> HistogramCounts;

RWTexture2D<float4> LuminanceMomentsOutput;

[numthreads(1, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
	const uint sampleCount = ExposureHistogram::CountSamples(HistogramCounts);
	const float averageLuminance = ExposureHistogram::ResolveAverageLuminance(HistogramCounts, sampleCount);
	LuminanceMomentsOutput[uint2(0u, 0u)] = float4(log(averageLuminance), sampleCount != 0u ? 1.0f : 0.0f, 0.0f, 0.0f);
}
