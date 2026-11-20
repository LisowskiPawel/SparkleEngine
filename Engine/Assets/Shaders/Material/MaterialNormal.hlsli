#pragma once

#include "/Engine/Common/Math.hlsli"
#include "/Engine/Material/MaterialTextureFlags.hlsli"

float3 DecodeTangentNormal(float3 encodedNormal, float normalScale, uint textureFlags)
{
	const float2 decodedNormalXY = encodedNormal.xy * 2.0f - 1.0f;
	const bool bRgbNormal = (textureFlags & MaterialTextureFlags::RgbNormal) != 0u;
	const float normalZ = bRgbNormal ? encodedNormal.z * 2.0f - 1.0f : sqrt(saturate(1.0f - dot(decodedNormalXY, decodedNormalXY)));
	return SafeNormalize(float3(decodedNormalXY * normalScale, normalZ));
}
