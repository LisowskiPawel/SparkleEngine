Texture2D DisplayLinearColor;
RWTexture2D<float4> OutputColor;

[numthreads(8, 8, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
	uint width;
	uint height;
	OutputColor.GetDimensions(width, height);
	const uint2 pixelCoord = dispatchThreadId.xy;
	if (pixelCoord.x >= width || pixelCoord.y >= height)
	{
		return;
	}
	float4 color = DisplayLinearColor.Load(int3(pixelCoord, 0));
	const float2 centeredUv = (float2(pixelCoord) + 0.5f) / float2(width, height) * 2.0f - 1.0f;
	const int2 offset = int2(round(centeredUv * dot(centeredUv, centeredUv) * 1.5f));
	color.r = DisplayLinearColor.Load(int3(clamp(int2(pixelCoord) + offset, int2(0, 0), int2(width, height) - 1), 0)).r;
	color.b = DisplayLinearColor.Load(int3(clamp(int2(pixelCoord) - offset, int2(0, 0), int2(width, height) - 1), 0)).b;
	OutputColor[pixelCoord] = color;
}
