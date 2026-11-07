#pragma once

namespace MaterialTextures
{
	static const uint TextureSlotBaseColor = 0u;
	static const uint TextureSlotNormal = 1u;
	static const uint TextureSlotRoughness = 2u;
	static const uint TextureSlotMetallic = 3u;
	static const uint TextureSlotOcclusion = 4u;
	static const uint TextureSlotEmissive = 5u;
	static const uint TextureSlotSubsurfaceColor = 6u;
	static const uint TextureSlotSubsurfaceStrength = 7u;

	uint TextureFlag(uint textureSlot)
	{
		return 1u << (textureSlot + 1u);
	}

	bool HasTexture(uint textureFlags, uint textureSlot)
	{
		return (textureFlags & TextureFlag(textureSlot)) != 0u;
	}
}
