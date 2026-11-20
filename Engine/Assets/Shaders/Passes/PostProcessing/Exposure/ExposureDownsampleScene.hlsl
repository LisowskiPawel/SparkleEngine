#include "/Engine/Passes/PostProcessing/Exposure/ExposureDownsampleCommon.hlsli"

Texture2D SceneColor;

[numthreads(8, 8, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
	const uint2 outputPixel = dispatchThreadId.xy;
	if (ExposureDownsample::IsOutsideOutput(outputPixel))
	{
		return;
	}

	const float2 moments = ExposureDownsample::SumMoments2x2(SceneColor, outputPixel, true);
	ExposureDownsample::Store(outputPixel, moments);
}
