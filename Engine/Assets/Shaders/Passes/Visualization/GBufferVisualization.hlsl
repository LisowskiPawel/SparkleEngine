#include "/Engine/Resources/ViewUniformData.hlsli"

RWTexture2D<float4> SceneColor;

#include "/Engine/Passes/Visualization/GBufferVisualization.hlsli"

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

	StoreGBufferVisualization(dispatchThreadId.xy, RenderViewModeIndex, SceneColor);
}
