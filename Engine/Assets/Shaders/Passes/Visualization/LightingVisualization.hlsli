#pragma once

#include "/Engine/Resources/RenderViewModeConstants.hlsli"

Texture2D GBufferBaseColor;
Texture2D DirectDiffuse;
Texture2D DirectSpecular;
Texture2D DirectSubsurface;
Texture2D IndirectDiffuse;
Texture2D IndirectSpecular;

void StoreLightingVisualization(uint2 pixelCoord, uint viewMode, RWTexture2D<float4> sceneColor)
{
	const int3 pixel = int3(pixelCoord, 0);
	float3 outputColor = 0.0f;
	switch (viewMode)
	{
		case RenderViewMode::DirectDiffuse:
			outputColor = DirectDiffuse.Load(pixel).rgb;
			break;

		case RenderViewMode::DirectSpecular:
			outputColor = DirectSpecular.Load(pixel).rgb;
			break;

		case RenderViewMode::DirectSubsurface:
			outputColor = DirectSubsurface.Load(pixel).rgb;
			break;

		case RenderViewMode::IndirectDiffuse:
			outputColor = IndirectDiffuse.Load(pixel).rgb;
			break;

		case RenderViewMode::IndirectSpecular:
			outputColor = IndirectSpecular.Load(pixel).rgb;
			break;
	}
	sceneColor[pixelCoord] = float4(max(outputColor, 0.0f), GBufferBaseColor.Load(pixel).a);
}
