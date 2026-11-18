#include "PCH.h"

#include "Resources/RhiTextureUpload.h"

#include <algorithm>
#include <bit>
#include <limits>

bool RhiTextureUploadDesc::IsValid() const noexcept
{
	if (Width == 0 || Height == 0 || ArraySize == 0)
	{
		return false;
	}
	if (ArraySize > (std::numeric_limits<std::uint16_t>::max)() || ArraySlices.size() != ArraySize)
	{
		return false;
	}
	if (Dimension != TextureResourceDimension::Texture2D && Dimension != TextureResourceDimension::TextureCube)
	{
		return false;
	}
	if (IsCube() && (ArraySize != 6 || Width != Height))
	{
		return false;
	}

	const std::size_t levels = ArraySlices.front().size();
	if (levels == 0 || levels > std::bit_width(std::max(Width, Height)))
	{
		return false;
	}
	if (!PixelFormatHasColorAspect(Format))
	{
		return false;
	}

	const bool bCompressed = PixelFormatBytesPerTexel(Format) == 0;
	for (const auto& slice : ArraySlices)
	{
		if (slice.size() != levels)
		{
			return false;
		}

		std::uint32_t width = Width;
		std::uint32_t height = Height;
		for (const auto& mip : slice)
		{
			const std::uint64_t rowBytes = PixelFormatRowBytes(Format, width);
			const std::uint64_t rows = bCompressed ? (static_cast<std::uint64_t>(height) + 3) / 4 : height;
			if (rowBytes == 0 || rowBytes > (std::numeric_limits<std::uint32_t>::max)())
			{
				return false;
			}
			if (mip.Width != width || mip.Height != height || mip.RowPitch != rowBytes)
			{
				return false;
			}
			if (mip.SlicePitch != rowBytes * rows || mip.Data.size() != mip.SlicePitch)
			{
				return false;
			}

			width = std::max(1u, width / 2);
			height = std::max(1u, height / 2);
		}
	}
	return true;
}
