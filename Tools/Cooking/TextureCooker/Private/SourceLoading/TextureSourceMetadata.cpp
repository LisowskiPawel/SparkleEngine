#include "PCH.h"

#include "SourceLoading/TextureSource.h"
#include "Processing/TextureProcessor.h"
#include "TextureCookRequestList.h"
#include "Textures/CookedTextureAsset.h"
#include "Core/Public/Diagnostics/Error.h"

#include <algorithm>
#include <bit>

using namespace DirectX;

namespace TextureSourceAdmission
{
	bool IsSupportedFormat(DXGI_FORMAT format)
	{
		switch (MakeLinear(format))
		{
			case DXGI_FORMAT_R8_UNORM:
			case DXGI_FORMAT_R16_UNORM:
			case DXGI_FORMAT_R8G8B8A8_UNORM:
			case DXGI_FORMAT_B8G8R8A8_UNORM:
			case DXGI_FORMAT_R16G16B16A16_UNORM:
			case DXGI_FORMAT_R16G16B16A16_FLOAT:
			case DXGI_FORMAT_R32G32B32A32_FLOAT:
			case DXGI_FORMAT_BC1_UNORM:
			case DXGI_FORMAT_BC2_UNORM:
			case DXGI_FORMAT_BC3_UNORM:
			case DXGI_FORMAT_BC4_UNORM:
			case DXGI_FORMAT_BC4_SNORM:
			case DXGI_FORMAT_BC5_UNORM:
			case DXGI_FORMAT_BC5_SNORM:
			case DXGI_FORMAT_BC6H_UF16:
			case DXGI_FORMAT_BC7_UNORM:
				return true;
			default:
				return false;
		}
	}

	void ValidateMetadata(const TexMetadata& metadata, const TextureCookRequest& request)
	{
		const bool bDimensionsInRange = metadata.width > 0 && metadata.width <= kMaximumCookedTextureDimension;
		const bool bHeightInRange = metadata.height > 0 && metadata.height <= kMaximumCookedTextureDimension;
		const bool bLayersInRange = metadata.arraySize > 0 && metadata.arraySize <= kMaximumCookedTextureArraySize;
		if (!bDimensionsInRange || !bHeightInRange || !bLayersInRange || metadata.dimension != TEX_DIMENSION_TEXTURE2D || metadata.depth != 1)
		{
			throw Diagnostics::Error("Texture metadata exceeds supported 2D/array dimensions.");
		}
		if (metadata.IsCubemap() && (metadata.arraySize != 6 || metadata.width != metadata.height))
		{
			throw Diagnostics::Error("A texture cube must contain exactly six square faces.");
		}
		if (metadata.IsCubemap() != request.IsCube())
		{
			throw Diagnostics::Error("Texture request dimension does not match its source container.");
		}
		if (metadata.mipLevels == 0 || metadata.mipLevels > std::bit_width(std::max(metadata.width, metadata.height)))
		{
			throw Diagnostics::Error("Source mip count exceeds its complete 2D chain.");
		}

		if (!IsSupportedFormat(metadata.format))
		{
			throw Diagnostics::Error("Unsupported source texture encoding.");
		}
	}
}

TexMetadata MakeFlatTextureMetadata(std::int64_t width, std::int64_t height, DXGI_FORMAT format)
{
	TexMetadata metadata{};
	metadata.width = width > 0 ? static_cast<std::size_t>(width) : 0;
	metadata.height = height > 0 ? static_cast<std::size_t>(height) : 0;
	metadata.depth = metadata.arraySize = metadata.mipLevels = 1;
	metadata.dimension = TEX_DIMENSION_TEXTURE2D;
	metadata.format = format;
	return metadata;
}

std::size_t MeasureTextureProcessingMemory(const TexMetadata& metadata, std::size_t fileBytes, const TextureCookRequest& request)
{
	TextureSourceAdmission::ValidateMetadata(metadata, request);

	std::uint64_t texels = 0;
	std::uint64_t nativeBytes = 0;
	std::size_t width = metadata.width;
	std::size_t height = metadata.height;
	for (std::size_t mip = 0; mip < metadata.mipLevels; ++mip)
	{
		texels += static_cast<std::uint64_t>(width) * height * metadata.arraySize;
		std::size_t row = 0;
		std::size_t slice = 0;
		const auto result = ComputePitch(metadata.format, width, height, row, slice);
		CheckTextureOperation(result, "measure source subresource");
		nativeBytes += slice * metadata.arraySize;
		width = std::max<std::size_t>(1, width / 2);
		height = std::max<std::size_t>(1, height / 2);
	}

	// Covers source-file/decode overlap, native float chains, transformations and encoded staging.
	const bool bPreserveBlocks = CanPreserveTextureBlocks(request, metadata.format);
	const std::uint64_t filterRows = (metadata.width + metadata.height) * metadata.arraySize * 256ull;
	const auto storage = bPreserveBlocks ? nativeBytes * 2ull : texels * 48ull + filterRows;
	const auto descriptors = metadata.arraySize * metadata.mipLevels * 4096ull;
	const std::uint64_t estimatedBytes = fileBytes * 2ull + storage + descriptors + 65536ull;
	return static_cast<std::size_t>(estimatedBytes);
}
