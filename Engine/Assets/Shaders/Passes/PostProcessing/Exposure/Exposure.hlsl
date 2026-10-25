#include "/Engine/Passes/PostProcessing/Exposure/ExposureResolve.hlsli"

Texture2D LuminanceMoments;
Texture2D PreviousExposureTexture;
RWTexture2D<float4> ExposureTexture;
RWTexture2D<float4> ExposureHistoryTexture;

[numthreads(1, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
	const float2 moments = LuminanceMoments.Load(int3(0, 0, 0)).xy;
	const float averageLuminance = Exposure::ResolveAverageLuminance(moments);
	const float4 previousPayload = ExposureHistoryValid != 0u ? PreviousExposureTexture.Load(int3(0, 0, 0)) : 0.0f.xxxx;
	const float4 exposurePayload = ResolveExposurePayload(averageLuminance, moments.y, previousPayload);
	ExposureTexture[uint2(0u, 0u)] = exposurePayload;
	ExposureHistoryTexture[uint2(0u, 0u)] = exposurePayload;
}
