#include "PCH.h"

#include "SourceLoading/TextureSource.h"
#include "Processing/TextureProcessor.h"
#include "Core/Public/Diagnostics/Error.h"

#include <ktx.h>

#include <cstring>
#include <memory>

using namespace DirectX;

namespace KtxSourceLoading
{
	struct Encoding final
	{
		std::uint32_t Gl;
		std::uint32_t Vk;
		DXGI_FORMAT Format;
	};

	DXGI_FORMAT ResolveEncoding(const ktxTexture& texture)
	{
		const bool bIsKtx2 = texture.classId == ktxTexture2_c;

		static constexpr Encoding encodings[] = {
		    {0x8058, 37, DXGI_FORMAT_R8G8B8A8_UNORM},
		    {0x8C43, 43, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB},
		    {0x8814, 109, DXGI_FORMAT_R32G32B32A32_FLOAT},
		    {0x83F1, 133, DXGI_FORMAT_BC1_UNORM},
		    {0x8C4D, 134, DXGI_FORMAT_BC1_UNORM_SRGB},
		    {0x83F2, 135, DXGI_FORMAT_BC2_UNORM},
		    {0x8C4E, 136, DXGI_FORMAT_BC2_UNORM_SRGB},
		    {0x83F3, 137, DXGI_FORMAT_BC3_UNORM},
		    {0x8C4F, 138, DXGI_FORMAT_BC3_UNORM_SRGB},
		    {0x8DBB, 139, DXGI_FORMAT_BC4_UNORM},
		    {0x8DBC, 140, DXGI_FORMAT_BC4_SNORM},
		    {0x8DBD, 141, DXGI_FORMAT_BC5_UNORM},
		    {0x8DBE, 142, DXGI_FORMAT_BC5_SNORM},
		    {0x8E8F, 143, DXGI_FORMAT_BC6H_UF16},
		    {0x8E8C, 145, DXGI_FORMAT_BC7_UNORM},
		    {0x8E8D, 146, DXGI_FORMAT_BC7_UNORM_SRGB}};

		for (const auto& encoding : encodings)
		{
			const auto sourceEncoding = bIsKtx2 ? reinterpret_cast<const ktxTexture2*>(&texture)->vkFormat : reinterpret_cast<const ktxTexture1*>(&texture)->glInternalformat;
			const auto expectedEncoding = bIsKtx2 ? encoding.Vk : encoding.Gl;
			const bool bMatches = sourceEncoding == expectedEncoding;
			if (bMatches)
			{
				return encoding.Format;
			}
		}
		return DXGI_FORMAT_UNKNOWN;
	}

	TexMetadata ReadMetadata(const ktxTexture& texture)
	{
		const bool bIsKtx2 = texture.classId == ktxTexture2_c;
		if (bIsKtx2 && reinterpret_cast<const ktxTexture2*>(&texture)->supercompressionScheme != KTX_SS_NONE)
		{
			throw Diagnostics::Error("Supercompressed KTX sources are unsupported; provide native texture blocks or pixels.");
		}

		const auto format = ResolveEncoding(texture);
		if (format == DXGI_FORMAT_UNKNOWN || texture.numDimensions != 2 || texture.baseDepth != 1)
		{
			throw Diagnostics::Error("Unsupported native KTX encoding or resource dimension.");
		}

		TexMetadata metadata{};
		metadata.width = texture.baseWidth;
		metadata.height = texture.baseHeight;
		metadata.depth = 1;
		metadata.arraySize = static_cast<std::size_t>(texture.numLayers) * texture.numFaces;
		metadata.mipLevels = texture.numLevels;
		metadata.format = format;
		metadata.dimension = TEX_DIMENSION_TEXTURE2D;
		metadata.miscFlags = texture.isCubemap ? TEX_MISC_TEXTURECUBE : 0;
		return metadata;
	}

	void LoadPixels(ktxTexture* texture, const TexMetadata& metadata, ScratchImage& images)
	{
		const auto loaded = ktxTexture_LoadImageData(texture, nullptr, 0);
		if (loaded != KTX_SUCCESS)
		{
			throw Diagnostics::Error("KTX payload load failed: " + std::string(ktxErrorString(loaded)));
		}

		CheckTextureOperation(images.Initialize(metadata), "allocate imported KTX subresources");
		for (std::size_t layer = 0; layer < texture->numLayers; ++layer)
		{
			for (std::size_t face = 0; face < texture->numFaces; ++face)
			{
				for (std::size_t mip = 0; mip < metadata.mipLevels; ++mip)
				{
					ktx_size_t offset = 0;
					const auto offsetResult = ktxTexture_GetImageOffset(texture, static_cast<ktx_uint32_t>(mip), static_cast<ktx_uint32_t>(layer), static_cast<ktx_uint32_t>(face), &offset);
					if (offsetResult != KTX_SUCCESS)
					{
						throw Diagnostics::Error("KTX subresource offset is invalid.");
					}
					const auto size = ktxTexture_GetImageSize(texture, static_cast<ktx_uint32_t>(mip));
					const Image& destination = *images.GetImage(mip, layer * texture->numFaces + face, 0);
					if (offset > texture->dataSize || size > texture->dataSize - offset || size != destination.slicePitch)
					{
						throw Diagnostics::Error("KTX subresource payload does not match its admitted native layout.");
					}

					std::memcpy(destination.pixels, texture->pData + offset, size);
				}
			}
		}
	}

	void OrientPixels(ScratchImage& images, TEX_FR_FLAGS flags)
	{
		if (flags == TEX_FR_ROTATE0)
		{
			return;
		}
		ScratchImage oriented;
		CheckTextureOperation(FlipRotate(images.GetImages(), images.GetImageCount(), images.GetMetadata(), flags, oriented), "orient imported KTX pixels");
		images = std::move(oriented);
	}
}

using namespace KtxSourceLoading;

TextureSource LoadKtxTextureSource(std::span<const std::uint8_t> bytes, const TextureCookRequest& request, TextureCookMemoryLimiter& processingMemory)
{
	ktxTexture* texture = nullptr;
	const auto created = ktxTexture_CreateFromMemory(bytes.data(), bytes.size(), KTX_TEXTURE_CREATE_NO_FLAGS, &texture);
	if (created != KTX_SUCCESS)
	{
		throw Diagnostics::Error("KTX metadata load failed: " + std::string(ktxErrorString(created)));
	}

	auto destroy = [](ktxTexture* value)
	{
		ktxTexture_Destroy(value);
	};

	const std::unique_ptr<ktxTexture, decltype(destroy)> owner(texture, destroy);

	const auto metadata = ReadMetadata(*texture);
	TextureSource source;
	source.ProcessingMemory = processingMemory.Acquire(MeasureTextureProcessingMemory(metadata, bytes.size(), request));
	const bool bFlipX = texture->orientation.x == KTX_ORIENT_X_LEFT;
	const bool bFlipY = texture->orientation.y == KTX_ORIENT_Y_UP;
	if (IsCompressed(metadata.format) && (bFlipX || bFlipY))
	{
		throw Diagnostics::Error("Compressed KTX sources require right/down orientation to preserve native blocks.");
	}

	LoadPixels(texture, metadata, source.Images);

	const auto flags = static_cast<TEX_FR_FLAGS>((bFlipX ? TEX_FR_FLIP_HORIZONTAL : 0) | (bFlipY ? TEX_FR_FLIP_VERTICAL : 0));
	OrientPixels(source.Images, flags);
	return source;
}
