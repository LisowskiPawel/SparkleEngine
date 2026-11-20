#include "PCH.h"
#include "Core/Public/Paths/CookedAssetPaths.h"

#include "Textures/CookedTextureLoader.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/BinarySpanReader.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Files/FileSetPublication.h"
#include "RHI/Public/Textures/CookedTextureAsset.h"

#include <format>
#include <algorithm>
#include <bit>
#include <limits>
#include <string>
#include <span>
#include <system_error>

static PixelFormat ValidateHeader(const CookedTextureAssetHeader& header, const std::filesystem::path& resolvedPath)
{
	if (!header.HasExpectedMagic())
	{
		throw Diagnostics::Error("Invalid cooked texture asset header for '" + resolvedPath.string() + "'");
	}

	const bool bWidthInRange = header.width > 0 && header.width <= kMaximumCookedTextureDimension;
	const bool bHeightInRange = header.height > 0 && header.height <= kMaximumCookedTextureDimension;
	const auto format = PixelFormatFromSerializedTextureFormat(header.format);
	if (!bWidthInRange || !bHeightInRange || !PixelFormatHasColorAspect(format))
	{
		throw Diagnostics::Error("Cooked texture asset header has invalid dimensions or format for '" + resolvedPath.string() + "'");
	}

	if (header.mipCount == 0 || header.mipCount > std::bit_width(std::max(header.width, header.height)))
	{
		throw Diagnostics::Error("Cooked texture asset has an invalid mip count for '" + resolvedPath.string() + "'.");
	}
	if (header.GetArraySize() == 0 || header.GetArraySize() > kMaximumCookedTextureArraySize)
	{
		throw Diagnostics::Error("Cooked texture asset header has an invalid array size for '" + resolvedPath.string() + "'");
	}

	const bool bUnsupportedTextureDimension = header.GetDimension() != TextureResourceDimension::Texture2D && header.GetDimension() != TextureResourceDimension::TextureCube;

	if (bUnsupportedTextureDimension)
	{
		throw Diagnostics::Error("Cooked texture asset header has an invalid texture dimension for '" + resolvedPath.string() + "'");
	}

	if (header.packedLayout != PackCookedTextureLayout(header.GetDimension(), header.GetArraySize()))
	{
		throw Diagnostics::Error("Cooked texture asset has reserved layout bits for '" + resolvedPath.string() + "'.");
	}
	if (header.GetDimension() == TextureResourceDimension::TextureCube && (header.GetArraySize() != 6 || header.width != header.height))
	{
		throw Diagnostics::Error("Cooked texture asset header has an invalid cubemap face count for '" + resolvedPath.string() + "'");
	}
	return format;
}

static void ValidateMipHeader(
    const CookedTextureMipHeader& mipHeader,
    const CookedTextureAssetHeader& header,
    PixelFormat format,
    std::uint32_t subresourceIndex,
    const std::filesystem::path& resolvedPath)
{
	const auto mipIndex = subresourceIndex % header.mipCount;
	const auto width = std::max(1u, header.width >> mipIndex);
	const auto height = std::max(1u, header.height >> mipIndex);
	if (mipHeader.width != width || mipHeader.height != height || mipHeader.reserved != 0)
	{
		throw Diagnostics::Error(std::format("Cooked texture asset '{}' has an invalid mip header at index {}", resolvedPath.string(), mipIndex));
	}
	const auto rowBytes = PixelFormatRowBytes(format, width);
	if (rowBytes == 0 || mipHeader.rowPitch != rowBytes)
	{
		throw Diagnostics::Error(std::format("Cooked texture asset '{}' has a noncanonical row pitch at mip {}.", resolvedPath.string(), mipIndex));
	}

	const auto rows = PixelFormatBytesPerTexel(format) == 0 ? (height + 3u) / 4u : height;
	if (mipHeader.slicePitch != rowBytes * rows || mipHeader.dataSize != mipHeader.slicePitch)
	{
		const std::string message = std::format(
		    "Cooked texture asset '{}' stores mip {} with {} payload bytes and slice pitch {}; expected {} bytes.",
		    resolvedPath.string(),
		    mipIndex,
		    mipHeader.dataSize,
		    mipHeader.slicePitch,
		    rowBytes * rows);

		throw Diagnostics::Error(message);
	}
}

static TextureFormatIntent ResolveFormatIntent(std::uint32_t storedValue, PixelFormat format)
{
	const auto intent = IsSrgbPixelFormat(format) ? TextureFormatIntent::ColorSrgb : TextureFormatIntent::DataLinear;
	if (storedValue != static_cast<std::uint32_t>(intent))
	{
		throw Diagnostics::Error(std::format("Cooked texture format intent {} contradicts its pixel format.", storedValue));
	}
	return intent;
}

static std::size_t CalculateMaximumCookedTextureFileBytes(std::uint64_t maximumDecodedBytes) noexcept
{
	constexpr auto maximumHeaders = sizeof(CookedTextureAssetHeader) + sizeof(CookedTextureMipHeader) * kMaximumCookedTextureMipCount * kMaximumCookedTextureArraySize;
	constexpr auto maximumPayload = (std::numeric_limits<std::size_t>::max)() - maximumHeaders;
	return static_cast<std::size_t>(std::min<std::uint64_t>(maximumDecodedBytes, maximumPayload)) + maximumHeaders;
}

std::uint64_t GetCookedTextureDecodeReservation(const std::filesystem::path& texturePath, std::uint64_t maximumDecodedBytes)
{
	std::error_code error;
	const auto fileBytes = std::filesystem::file_size(texturePath, error);
	if (error)
	{
		throw Diagnostics::Error(std::format("Could not measure cooked texture '{}': {}", texturePath.string(), error.message()));
	}
	if (fileBytes == 0 || fileBytes > CalculateMaximumCookedTextureFileBytes(maximumDecodedBytes))
	{
		throw Diagnostics::Error(std::format("Cooked texture '{}' exceeds the {}-byte texture decode allowance or is empty.", texturePath.string(), maximumDecodedBytes));
	}
	// This is a size bound, not a content identity; the guarded read and decode enforce it again.
	return std::min<std::uint64_t>(fileBytes, maximumDecodedBytes);
}

CookedTextureFilePayload ReadCookedTexturePayload(const std::filesystem::path& texturePath, std::uint64_t maximumDecodedBytes)
{
	CookedTextureFilePayload payload;
	const auto resolvedPath = Filesystem::ResolveCookedTexturePath(texturePath);
	if (!resolvedPath)
	{
		throw Diagnostics::Error("Cooked texture not found or not a .stex product: '" + texturePath.string() + "'.");
	}
	payload.ResolvedPath = *resolvedPath;
	const Files::FileSetReadScope publication(payload.ResolvedPath.parent_path());
	const auto maximumFileBytes = CalculateMaximumCookedTextureFileBytes(maximumDecodedBytes);
	std::string errorMessage;
	if (!Files::TryReadAllBytes(payload.ResolvedPath, payload.Bytes, errorMessage, maximumFileBytes))
	{
		throw Diagnostics::Error(std::format("Could not read cooked texture '{}': {}", payload.ResolvedPath.string(), errorMessage));
	}
	return payload;
}

LoadedTextureData DecodeCookedTexture(const CookedTextureFilePayload& payload, std::uint64_t maximumDecodedBytes)
{
	if (payload.ResolvedPath.empty() || payload.Bytes.empty())
	{
		throw Diagnostics::Error("Cooked texture decode received an empty file payload.");
	}

	Files::BinarySpanReader reader(payload.Bytes);
	CookedTextureAssetHeader header;
	std::string errorMessage;
	if (!reader.ReadValue(header, errorMessage))
	{
		throw Diagnostics::Error(errorMessage);
	}
	const auto format = ValidateHeader(header, payload.ResolvedPath);

	const TextureFormatIntent formatIntent = ResolveFormatIntent(header.formatIntent, format);

	std::vector<CookedTextureMipHeader> mipHeaders;
	const std::size_t subresourceCount = static_cast<std::size_t>(header.mipCount) * header.GetArraySize();
	if (!reader.ReadArray(subresourceCount, mipHeaders, errorMessage))
	{
		throw Diagnostics::Error(errorMessage);
	}

	std::uint64_t decodedBytes = 0;
	for (std::uint32_t mipIndex = 0; mipIndex < static_cast<std::uint32_t>(mipHeaders.size()); ++mipIndex)
	{
		ValidateMipHeader(mipHeaders[mipIndex], header, format, mipIndex, payload.ResolvedPath);
		if (mipHeaders[mipIndex].dataSize > maximumDecodedBytes - decodedBytes)
		{
			throw Diagnostics::Error(std::format("Cooked texture asset '{}' exceeds its decoded-byte reservation.", payload.ResolvedPath.string()));
		}
		decodedBytes += mipHeaders[mipIndex].dataSize;
	}
	if (decodedBytes != reader.GetRemainingByteCount())
	{
		throw Diagnostics::Error(std::format("Cooked texture asset '{}' payload length does not match its subresources.", payload.ResolvedPath.string()));
	}

	RhiTextureUploadDesc textureUpload;
	textureUpload.Width = header.width;
	textureUpload.Height = header.height;
	textureUpload.ArraySize = header.GetArraySize();
	textureUpload.Dimension = header.GetDimension();
	textureUpload.Format = format;
	textureUpload.ArraySlices.resize(textureUpload.ArraySize);
	for (auto& slice : textureUpload.ArraySlices)
	{
		slice.reserve(header.mipCount);
	}

	const std::uint32_t mipCount = header.mipCount;
	for (std::uint32_t subresourceIndex = 0; subresourceIndex < static_cast<std::uint32_t>(mipHeaders.size()); ++subresourceIndex)
	{
		const CookedTextureMipHeader& mipHeader = mipHeaders[subresourceIndex];
		RhiTextureMipUploadData mipLevel;
		mipLevel.Width = mipHeader.width;
		mipLevel.Height = mipHeader.height;
		mipLevel.RowPitch = mipHeader.rowPitch;
		mipLevel.SlicePitch = mipHeader.slicePitch;

		std::span<const std::uint8_t> mipPayload;
		if (!reader.ReadBytes(mipHeader.dataSize, mipPayload, errorMessage))
		{
			throw Diagnostics::Error(std::format("Cooked texture asset '{}' ended before mip payload data could be read.", payload.ResolvedPath.string()));
		}
		mipLevel.Data.assign(mipPayload.begin(), mipPayload.end());

		const std::uint32_t arraySliceIndex = subresourceIndex / mipCount;
		textureUpload.ArraySlices[arraySliceIndex].push_back(std::move(mipLevel));
	}

	return LoadedTextureData{.Upload = std::move(textureUpload), .FormatIntent = formatIntent};
}
