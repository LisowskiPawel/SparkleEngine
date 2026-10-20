#pragma once

#include "/Engine/Resources/ObjectShaderData.hlsli"

#include "/Engine/Resources/ViewCameraUniformData.hlsli"

float4 PositionLocalToWorld(float4 localPosition)
{
	return mul(localPosition, WorldMatrix);
}

float4 PositionWorldToView(float4 worldPosition)
{
	return mul(worldPosition, ViewMTX);
}

float4 PositionViewToClip(float4 viewPosition)
{
	return mul(viewPosition, ProjectionMTX);
}

float4 PositionWorldToClip(float4 worldPosition)
{
	return mul(worldPosition, ViewProjMTX);
}

float4 ApplyTemporalJitterClipOffset(float4 clipPosition, float2 jitterNdc)
{
	clipPosition.xy += jitterNdc * clipPosition.w;
	return clipPosition;
}
