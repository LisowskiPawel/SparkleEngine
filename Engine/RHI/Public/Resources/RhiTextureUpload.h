#pragma once

#include "../Formats/PixelFormat.h"
#include "TextureTypes.h"

#include <cstddef>
#include <cstdint>
#include <vector>

struct RhiTextureMipUploadData
{
	std::uint32_t Width = 1;
	std::uint32_t Height = 1;
	std::uint32_t RowPitch = 0;
	std::uint32_t SlicePitch = 0;
	std::vector<std::uint8_t> Data;
};

using RhiTextureArraySliceUploadData = std::vector<RhiTextureMipUploadData>;

struct RhiTextureUploadDesc
{
	std::uint32_t Width = 1;
	std::uint32_t Height = 1;
	std::uint32_t ArraySize = 1;
	TextureResourceDimension Dimension = TextureResourceDimension::Texture2D;
	PixelFormat Format = PixelFormat::Unknown;
	std::vector<RhiTextureArraySliceUploadData> ArraySlices;

	std::uint16_t GetMipCount() const noexcept { return ArraySlices.empty() ? 0 : static_cast<std::uint16_t>(ArraySlices.front().size()); }

	std::uint16_t GetArraySize() const noexcept { return static_cast<std::uint16_t>(ArraySize); }

	std::uint32_t GetSubresourceCount() const noexcept { return static_cast<std::uint32_t>(GetMipCount()) * ArraySize; }

	bool IsCube() const noexcept { return Dimension == TextureResourceDimension::TextureCube; }

	SPARKLE_RHI_API bool IsValid() const noexcept;
};
