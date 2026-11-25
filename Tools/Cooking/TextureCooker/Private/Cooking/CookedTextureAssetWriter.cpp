#include "PCH.h"
#include "Cooking/CookedTextureAssetWriter.h"
#include "Textures/CookedTextureAsset.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/BinaryFileWriter.h"

#include <DirectXTex.h>

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
				CookedTextureMipHeader header{};
				header.width = static_cast<std::uint32_t>(image->width);
				header.height = static_cast<std::uint32_t>(image->height);
				header.rowPitch = static_cast<std::uint32_t>(image->rowPitch);
				header.slicePitch = header.dataSize = static_cast<std::uint32_t>(image->slicePitch);
				mipHeaders.push_back(header);
			}
		}

		return mipHeaders;
	}

	void WriteContents(Files::BinaryFileWriter& output, const DirectX::ScratchImage& texture, std::span<const CookedTextureMipHeader> mipHeaders)
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

		output.WriteValue(header);
		output.WriteBytes(mipHeaders.data(), mipHeaders.size() * sizeof(CookedTextureMipHeader));

		for (std::size_t index = 0; index < mipHeaders.size(); ++index)
		{
			const auto& image = *texture.GetImage(index % levels, index / levels, 0);
			output.WriteBytes(image.pixels, image.slicePitch);
		}
	}
}

void StageCookedTextureAsset(const DirectX::ScratchImage& texture, TextureMipPolicy policy, const std::filesystem::path& stagedOutputPath)
{
	const auto& metadata = texture.GetMetadata();
	const std::size_t levels = policy == TextureMipPolicy::NoMips ? 1 : metadata.mipLevels;
	const auto mipHeaders = CookedTextureWriting::BuildMipHeaders(texture, levels);

	Files::BinaryFileWriter output(stagedOutputPath);
	CookedTextureWriting::WriteContents(output, texture, mipHeaders);
	output.Close();
}
