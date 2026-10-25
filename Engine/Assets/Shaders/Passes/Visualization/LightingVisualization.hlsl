#include "/Engine/Resources/ViewUniformData.hlsli"

RWTexture2D<float4> SceneColor;

#include "/Engine/Passes/Visualization/LightingVisualization.hlsli"

[numthreads(8, 8, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
	uint width, height;
	SceneColor.GetDimensions(width, height);
	if (dispatchThreadId.x >= width || dispatchThreadId.y >= height)
	{
		return;
	}

	StoreLightingVisualization(dispatchThreadId.xy, RenderViewModeIndex, SceneColor);
}
