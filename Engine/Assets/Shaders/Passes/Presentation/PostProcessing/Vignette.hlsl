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
	const float attenuation = 1.0f - 0.2f * saturate(dot(centeredUv, centeredUv) * 0.5f);
	color.rgb *= attenuation;
	OutputColor[pixelCoord] = color;
}
