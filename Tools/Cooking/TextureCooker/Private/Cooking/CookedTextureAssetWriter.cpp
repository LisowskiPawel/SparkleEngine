#include "PCH.h"
#include "Cooking/CookedTextureAssetWriter.h"
#include "Processing/TextureProcessor.h"
#include "Textures/CookedTextureAsset.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/BinaryStreamWriter.h"
#include "Core/Public/Files/FileUtils.h"

#include <fstream>
#include <limits>
#include <span>
#include <vector>

namespace CookedTextureWriting
{
	std::vector<CookedTextureMipHeader> BuildMipHeaders(const DirectX::ScratchImage& texture, std::size_t levels)
	{
		const auto& metadata = texture.GetMetadata();
		std::vector<CookedTextureMipHeader> mipHeaders;
		mipHeaders.reserve(metadata.arraySize * levels);
		for (std::size_t item = 0; item < metadata.arraySize; ++item)
		{
			for (std::size_t mip = 0; mip < levels; ++mip)
			{
				const auto* image = texture.GetImage(mip, item, 0);
				if (!image || !image->pixels || image->slicePitch > (std::numeric_limits<std::uint32_t>::max)())
				{
					throw Diagnostics::Error("Native output contains an invalid or oversized subresource.");
				}
				std::size_t row = 0;
				std::size_t slice = 0;
				CheckTextureOperation(DirectX::ComputePitch(metadata.format, image->width, image->height, row, slice), "measure output subresource");
				if (image->rowPitch != row || image->slicePitch != slice)
				{
					throw Diagnostics::Error("Cooked textures require canonical tightly packed native rows.");
				}
				CookedTextureMipHeader header{};
				header.width = static_cast<std::uint32_t>(image->width);
				header.height = static_cast<std::uint32_t>(image->height);
				header.rowPitch = static_cast<std::uint32_t>(row);
				header.slicePitch = header.dataSize = static_cast<std::uint32_t>(slice);
				mipHeaders.push_back(header);
			}
		}

		return mipHeaders;
	}

	void WriteContents(std::ofstream& output, const DirectX::ScratchImage& texture, std::span<const CookedTextureMipHeader> mipHeaders)
	{
		const auto& metadata = texture.GetMetadata();
		const auto levels = mipHeaders.size() / metadata.arraySize;
		CookedTextureAssetHeader header{};
		header.magic = kCookedTextureAssetMagic;
		header.width = static_cast<std::uint32_t>(metadata.width);
		header.height = static_cast<std::uint32_t>(metadata.height);
		header.format = static_cast<std::uint32_t>(metadata.format);
		header.formatIntent = static_cast<std::uint32_t>(DirectX::IsSRGB(metadata.format) ? TextureFormatIntent::ColorSrgb : TextureFormatIntent::DataLinear);
		header.mipCount = static_cast<std::uint16_t>(levels);
		const auto dimension = metadata.IsCubemap() ? TextureResourceDimension::TextureCube : TextureResourceDimension::Texture2D;
		header.packedLayout = PackCookedTextureLayout(dimension, static_cast<std::uint16_t>(metadata.arraySize));

		std::string error;
		const bool bHeaderWritten = Files::BinaryStreamWriter::WriteValue(output, header, error);
		const bool bMipHeadersWritten = bHeaderWritten && Files::BinaryStreamWriter::WriteBytes(output, mipHeaders.data(), mipHeaders.size() * sizeof(CookedTextureMipHeader), error);
		if (!bMipHeadersWritten)
		{
			throw Diagnostics::Error(error);
		}

		for (std::size_t index = 0; index < mipHeaders.size(); ++index)
		{
			const auto& image = *texture.GetImage(index % levels, index / levels, 0);
			const bool bWritten = Files::BinaryStreamWriter::WriteBytes(output, image.pixels, image.slicePitch, error);
			if (!bWritten)
			{
				throw Diagnostics::Error(error);
			}
		}
	}
}

void StageCookedTextureAsset(const DirectX::ScratchImage& texture, TextureMipPolicy policy, const std::filesystem::path& stagedOutputPath)
{
	const auto& metadata = texture.GetMetadata();
	const std::size_t levels = policy == TextureMipPolicy::NoMips ? 1 : metadata.mipLevels;
	const auto mipHeaders = CookedTextureWriting::BuildMipHeaders(texture, levels);

	std::ofstream output;
	std::string error;
	if (!Files::TryOpenBinaryOutput(stagedOutputPath, output, error))
	{
		throw Diagnostics::Error(error);
	}
	CookedTextureWriting::WriteContents(output, texture, mipHeaders);
	if (!Files::TryCloseOutput(output, stagedOutputPath, error))
	{
		throw Diagnostics::Error(error);
	}
}
