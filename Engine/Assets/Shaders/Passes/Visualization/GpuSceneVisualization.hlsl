RWTexture2D<float4> SceneColor;
#include "/Engine/Common/Hash.hlsli"
Texture2D<uint> GBufferInstanceIdentity;

[numthreads(8, 8, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
	uint width = 0;
	uint height = 0;
	SceneColor.GetDimensions(width, height);

	if (dispatchThreadId.x >= width || dispatchThreadId.y >= height)
	{
		return;
	}

	const uint identity = GBufferInstanceIdentity.Load(int3(dispatchThreadId.xy, 0));
	const float3 color = identity == 0u ? 0.0f.xxx : HashIdColor(identity - 1u, 0u);
	SceneColor[dispatchThreadId.xy] = float4(color, identity == 0u ? 0.0f : 1.0f);
}
