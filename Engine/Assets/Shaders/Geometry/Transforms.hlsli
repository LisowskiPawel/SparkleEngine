#pragma once

#include "/Engine/Resources/ViewCameraUniformData.hlsli"

float4 PositionWorldToClip(float4 worldPosition)
{
	return mul(worldPosition, ViewProjMTX);
}

float4 ApplyTemporalJitterClipOffset(float4 clipPosition, float2 jitterNdc)
{
	clipPosition.xy += jitterNdc * clipPosition.w;
	return clipPosition;
}
