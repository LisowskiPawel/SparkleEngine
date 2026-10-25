#include "PCH.h"
#include "Cooking/CookedTextureAssetWriter.h"

#include "Pipeline/TextureLoadResult.h"
#include "Textures/CookedTextureAsset.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/BinaryStreamWriter.h"
#include "Core/Public/Files/FileUtils.h"

#include <fstream>
#include <limits>
#include <vector>

static std::vector<CookedTextureMipHeader> BuildMipHeaders(const TextureLoadResult& cookedTexture)
{
	std::vector<CookedTextureMipHeader> mipHeaders;
	mipHeaders.reserve(cookedTexture.GetSubresourceCount());
	for (const TextureArraySliceData& arraySlice : cookedTexture.arraySlices)
	{
		for (const TextureMipLevelData& mipLevel : arraySlice)
		{
			if (mipLevel.data.size() > (std::numeric_limits<std::uint32_t>::max)())
			{
				throw Diagnostics::Error("Texture mip payload is too large to serialize into a cooked texture asset.");
			}

			CookedTextureMipHeader mipHeader;
			mipHeader.width = mipLevel.width;
			mipHeader.height = mipLevel.height;
			mipHeader.rowPitch = mipLevel.rowPitch;
			mipHeader.slicePitch = mipLevel.slicePitch;
			mipHeader.dataSize = static_cast<std::uint32_t>(mipLevel.data.size());
			mipHeaders.push_back(mipHeader);
		}
	}
	return mipHeaders;
}

static CookedTextureAssetHeader BuildHeader(const TextureLoadResult& cookedTexture)
{
	CookedTextureAssetHeader header;
	header.magic = kCookedTextureAssetMagic;
	header.width = cookedTexture.width;
	header.height = cookedTexture.height;
	header.format = static_cast<std::uint32_t>(cookedTexture.dxgiFormat);
	header.formatIntent = static_cast<std::uint32_t>(cookedTexture.formatIntent);
	header.mipCount = cookedTexture.GetMipCount();
	header.packedLayout = PackCookedTextureLayout(cookedTexture.dimension, cookedTexture.GetArraySize());
	return header;
}

static void WriteMipPayloads(std::ostream& output, const TextureLoadResult& cookedTexture, std::string& fileError)
{
	for (const TextureArraySliceData& arraySlice : cookedTexture.arraySlices)
	{
		for (const TextureMipLevelData& mipLevel : arraySlice)
		{
			if (!Files::BinaryStreamWriter::WriteBytes(output, mipLevel.data.data(), mipLevel.data.size(), fileError))
			{
				throw Diagnostics::Error(fileError);
			}
		}
	}
}

void WriteCookedTextureAsset(const TextureLoadResult& cookedTexture, const std::filesystem::path& outputPath)
{
	const std::vector<CookedTextureMipHeader> mipHeaders = BuildMipHeaders(cookedTexture);
	const CookedTextureAssetHeader header = BuildHeader(cookedTexture);
	const std::filesystem::path temporaryOutputPath = Files::BuildTemporaryPath(outputPath);
	Files::CleanupTemporaryFile(temporaryOutputPath);

	std::ofstream output;
	std::string fileError;
	try
	{
		if (!Files::TryOpenBinaryOutput(temporaryOutputPath, output, fileError))
		{
			throw Diagnostics::Error(fileError);
		}

		if (!Files::BinaryStreamWriter::WriteValue(output, header, fileError)
		    || !Files::BinaryStreamWriter::WriteBytes(output, mipHeaders.data(), sizeof(CookedTextureMipHeader) * mipHeaders.size(), fileError))
		{
			throw Diagnostics::Error(fileError);
		}

		WriteMipPayloads(output, cookedTexture, fileError);

		output.flush();
		if (!output.good())
		{
			throw Diagnostics::Error("Failed to flush cooked texture output '" + temporaryOutputPath.string() + "'.");
		}

		if (!Files::TryCloseOutput(output, temporaryOutputPath, fileError))
		{
			throw Diagnostics::Error(fileError);
		}

		if (!Files::TryFinalizeTemporaryFile(temporaryOutputPath, outputPath, fileError))
		{
			throw Diagnostics::Error(fileError);
		}
	}
	catch (...)
	{
		Files::CleanupTemporaryFile(temporaryOutputPath, &output);
		throw;
	}
}
