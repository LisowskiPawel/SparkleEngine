#include "/Engine/Passes/GBuffer/SceneDepthUtils.hlsli"

RWTexture2D<float4> SceneColor;
Texture2D DirectDiffuse;
Texture2D DirectSpecular;
Texture2D DirectSubsurface;
Texture2D IndirectDiffuse;
Texture2D IndirectSpecular;
Texture2D GBufferBaseColor;
Texture2D GBufferDeviceZ;
Texture2D GBufferEmissive;

[numthreads(8, 8, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
	uint width, height;
	SceneColor.GetDimensions(width, height);
	if (dispatchThreadId.x >= width || dispatchThreadId.y >= height)
	{
		return;
	}

	const int3 pixel = int3(dispatchThreadId.xy, 0);
	const float deviceZ = GBufferDeviceZ.Load(pixel).r;
	if (SceneDepthUtils::IsSkyDeviceZ(deviceZ))
	{
		return;
	}

	const float3 emissive = GBufferEmissive.Load(pixel).rgb;
	const float3 directDiffuse = DirectDiffuse.Load(pixel).rgb;
	const float3 directSpecular = DirectSpecular.Load(pixel).rgb;
	const float3 directSubsurface = DirectSubsurface.Load(pixel).rgb;
	const float3 indirectDiffuse = IndirectDiffuse.Load(pixel).rgb;
	const float3 indirectSpecular = IndirectSpecular.Load(pixel).rgb;
	const float alpha = GBufferBaseColor.Load(pixel).a;

	float3 lit = max(emissive, 0.0f);
	lit += directDiffuse + directSpecular + directSubsurface;
	lit += indirectDiffuse + indirectSpecular;
	SceneColor[dispatchThreadId.xy] = float4(lit, alpha);
}
