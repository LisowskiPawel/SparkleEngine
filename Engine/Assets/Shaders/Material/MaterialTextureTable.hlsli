#pragma once

#include "/Engine/Material/MaterialTextureSlots.hlsli"

namespace MaterialTextures
{
	static const float ExplicitLod = 0.0f;
	static const uint AddressRepeat = 0u;
	static const uint AddressClampToEdge = 1u;
	static const uint AddressMirroredRepeat = 2u;

	uint ResolveTextureIndex(uint4 textureIndices0, uint4 textureIndices1, uint textureSlot)
	{
		return textureSlot < 4u ? textureIndices0[textureSlot] : textureIndices1[textureSlot - 4u];
	}

	float2 TransformUv(float2 uv, float4 linearTransform, float2 offset)
	{
		return float2(linearTransform.x * uv.x + linearTransform.y * uv.y, linearTransform.z * uv.x + linearTransform.w * uv.y) + offset;
	}

	int AddressTexel(int texel, uint extent, uint mode)
	{
		const int size = int(extent);
		if (mode == AddressClampToEdge)
		{
			return clamp(texel, 0, size - 1);
		}
		if (mode == AddressMirroredRepeat)
		{
			const int period = size * 2;
			const int wrapped = ((texel % period) + period) % period;
			return wrapped < size ? wrapped : period - 1 - wrapped;
		}

		return ((texel % size) + size) % size;
	}

	float4 SampleBaseLevelBilinear(Texture2D table[4096], uint textureIndex, float2 uv, uint addressModes)
	{
		uint width;
		uint height;
		table[NonUniformResourceIndex(textureIndex)].GetDimensions(width, height);
		const float2 texelPosition = uv * float2(width, height) - 0.5f;
		const int2 first = int2(floor(texelPosition));
		const float2 weight = frac(texelPosition);
		const uint addressU = addressModes & 3u;
		const uint addressV = (addressModes >> 2u) & 3u;
		const int2 p00 = int2(AddressTexel(first.x, width, addressU), AddressTexel(first.y, height, addressV));
		const int2 p10 = int2(AddressTexel(first.x + 1, width, addressU), p00.y);
		const int2 p01 = int2(p00.x, AddressTexel(first.y + 1, height, addressV));
		const int2 p11 = int2(p10.x, p01.y);
		const float4 row0 = lerp(table[NonUniformResourceIndex(textureIndex)].Load(int3(p00, 0)), table[NonUniformResourceIndex(textureIndex)].Load(int3(p10, 0)), weight.x);
		const float4 row1 = lerp(table[NonUniformResourceIndex(textureIndex)].Load(int3(p01, 0)), table[NonUniformResourceIndex(textureIndex)].Load(int3(p11, 0)), weight.x);
		return lerp(row0, row1, weight.y);
	}
}
