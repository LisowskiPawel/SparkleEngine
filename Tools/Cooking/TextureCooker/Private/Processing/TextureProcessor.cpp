#include "PCH.h"

#include "Processing/TextureProcessor.h"
#include "Core/Public/Diagnostics/Error.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <format>
#include <vector>

using namespace DirectX;

namespace TextureProcessing
{
	DXGI_FORMAT ApplyOutputIntent(DXGI_FORMAT format, const TextureCookRequest& request)
	{
		const auto output = request.IsSrgb() ? MakeSRGB(format) : MakeLinear(format);
		if (request.IsSrgb() && !IsSRGB(output))
		{
			throw Diagnostics::Error("The selected texture encoding cannot represent sRGB output intent.");
		}

		return output;
	}

	std::vector<Image> SelectImages(const ScratchImage& texture, TextureMipPolicy policy)
	{
		const auto& metadata = texture.GetMetadata();
		const std::size_t levels = policy == TextureMipPolicy::PreserveExisting ? metadata.mipLevels : 1;
		std::vector<Image> images;
		images.reserve(metadata.arraySize * levels);
		for (std::size_t item = 0; item < metadata.arraySize; ++item)
		{
			for (std::size_t mip = 0; mip < levels; ++mip)
			{
				images.push_back(*texture.GetImage(mip, item, 0));
			}
		}
		return images;
	}

	TexMetadata SelectMetadata(const ScratchImage& texture, TextureMipPolicy policy)
	{
		TexMetadata metadata = texture.GetMetadata();
		if (policy != TextureMipPolicy::PreserveExisting)
		{
			metadata.mipLevels = 1;
		}
		return metadata;
	}

	ScratchImage DecodeWorkingPixels(ScratchImage& source, const TextureCookRequest& request)
	{
		ScratchImage pixels;
		const auto format = source.GetMetadata().format;
		if (format == DXGI_FORMAT_R32G32B32A32_FLOAT)
		{
			return std::move(source);
		}
		source.OverrideFormat(MakeLinear(format));
		const auto images = SelectImages(source, request.policy.mipPolicy);
		const auto metadata = SelectMetadata(source, request.policy.mipPolicy);
		if (IsCompressed(format))
		{
			CheckTextureOperation(Decompress(images.data(), images.size(), metadata, DXGI_FORMAT_R32G32B32A32_FLOAT, pixels), "decompress source");
		}
		else
		{
			CheckTextureOperation(Convert(images.data(), images.size(), metadata, DXGI_FORMAT_R32G32B32A32_FLOAT, TEX_FILTER_DEFAULT, TEX_THRESHOLD_DEFAULT, pixels), "convert source");
		}
		source.Release();
		return pixels;
	}

	XMVECTOR DecodeBc5Normal(XMVECTOR pixel, bool bSigned)
	{
		const auto xy = bSigned ? pixel : XMVectorSubtract(XMVectorScale(pixel, 2.0f), g_XMOne);
		const auto z = XMVectorSqrt(XMVectorMax(XMVectorSubtract(g_XMOne, XMVector2LengthSq(xy)), g_XMZero));
		const auto normal = XMVectorSetZ(xy, XMVectorGetX(z));
		return XMVectorSetW(XMVectorMultiplyAdd(normal, g_XMOneHalf, g_XMOneHalf), XMVectorGetW(pixel));
	}

	void ApplySourcePolicy(ScratchImage& pixels, const TextureCookRequest& request, DXGI_FORMAT sourceFormat, bool bFloatValues)
	{
		const bool bDecodeSrgb = !bFloatValues && request.policy.colorProcessingPolicy == TextureColorProcessingPolicy::SrgbLinearize;
		const bool bExtractChannel = request.policy.channelMask != TextureChannelMask::Rgba;
		const bool bBc5 = sourceFormat == DXGI_FORMAT_BC5_UNORM || sourceFormat == DXGI_FORMAT_BC5_SNORM;
		const bool bNormalBc5 = bBc5 && request.policy.textureGroup == TextureGroup::NormalMap;
		const bool bSignedBc5 = sourceFormat == DXGI_FORMAT_BC5_SNORM;
		const bool bUnchangedChannels = !bDecodeSrgb && !bExtractChannel;
		if (bUnchangedChannels && !bNormalBc5)
		{
			return;
		}
		const unsigned channel = static_cast<unsigned>(request.policy.channelMask) - static_cast<unsigned>(TextureChannelMask::Red);
		const auto images = SelectImages(pixels, request.policy.mipPolicy);
		const auto metadata = SelectMetadata(pixels, request.policy.mipPolicy);
		ScratchImage extracted;

		CheckTextureOperation(
		    TransformImage(
		        images.data(),
		        images.size(),
		        metadata,
		        [channel, bDecodeSrgb, bExtractChannel, bNormalBc5, bSignedBc5](XMVECTOR* output, const XMVECTOR* input, std::size_t width, std::size_t)
		        {
			        for (std::size_t x = 0; x < width; ++x)
			        {
				        auto pixel = bDecodeSrgb ? XMColorSRGBToRGB(input[x]) : input[x];
				        if (bNormalBc5)
				        {
					        pixel = DecodeBc5Normal(pixel, bSignedBc5);
				        }
				        output[x] = bExtractChannel ? XMVectorSetW(XMVectorReplicate(XMVectorGetByIndex(pixel, channel)), 1.0f) : pixel;
			        }
		        },
		        extracted),
		    "apply source pixel semantics");

		pixels = std::move(extracted);
	}

	void NormalizeGeneratedNormals(ScratchImage& pixels)
	{
		const auto& metadata = pixels.GetMetadata();
		for (std::size_t item = 0; item < metadata.arraySize; ++item)
		{
			for (std::size_t mip = 1; mip < metadata.mipLevels; ++mip)
			{
				const Image& image = *pixels.GetImage(mip, item, 0);
				ScratchImage normalized;

				CheckTextureOperation(
				    TransformImage(
				        image,
				        [](XMVECTOR* output, const XMVECTOR* input, std::size_t width, std::size_t)
				        {
					        for (std::size_t x = 0; x < width; ++x)
					        {
						        auto normal = XMVectorSubtract(XMVectorScale(input[x], 2.0f), XMVectorReplicate(1.0f));
						        normal = XMVector3Normalize(normal);
						        normal = XMVectorScale(XMVectorAdd(normal, XMVectorReplicate(1.0f)), 0.5f);
						        output[x] = XMVectorSelect(normal, input[x], XMVectorSelectControl(0, 0, 0, 1));
					        }
				        },
				        normalized),
				    "normalize generated normals");

				std::memcpy(image.pixels, normalized.GetPixels(), image.slicePitch);
			}
		}
	}

	void GenerateMips(ScratchImage& pixels, const TextureCookRequest& request)
	{
		if (request.policy.mipPolicy != TextureMipPolicy::Generate)
		{
			return;
		}
		const auto& sourceMetadata = pixels.GetMetadata();
		if (sourceMetadata.width == 1 && sourceMetadata.height == 1)
		{
			return;
		}
		const auto group = request.policy.textureGroup;
		const bool bStandardColor = group == TextureGroup::Diffuse || group == TextureGroup::Emissive;
		const bool bCubicColor = bStandardColor || group == TextureGroup::SubsurfaceColor;
		const auto filter = bCubicColor ? TEX_FILTER_CUBIC : TEX_FILTER_LINEAR;
		const auto flags = static_cast<TEX_FILTER_FLAGS>(filter | TEX_FILTER_FORCE_NON_WIC | TEX_FILTER_SEPARATE_ALPHA);
		ScratchImage generated;
		CheckTextureOperation(GenerateMipMaps(pixels.GetImages(), pixels.GetImageCount(), pixels.GetMetadata(), flags, 0, generated), "generate mip chain");
		pixels = std::move(generated);
		if (group == TextureGroup::NormalMap)
		{
			NormalizeGeneratedNormals(pixels);
		}
	}

	struct PixelProperties final
	{
		bool bHasAlpha = false;
		bool bIsGray = true;
		bool bFitsUnsignedHalf = true;
	};

	PixelProperties InspectPixels(const ScratchImage& pixels, TextureMipPolicy policy)
	{
		PixelProperties properties;
		const auto images = SelectImages(pixels, policy);
		const auto metadata = SelectMetadata(pixels, policy);

		CheckTextureOperation(
		    EvaluateImage(
		        images.data(),
		        images.size(),
		        metadata,
		        [&properties](const XMVECTOR* input, std::size_t width, std::size_t)
		        {
			        for (std::size_t x = 0; x < width; ++x)
			        {
				        if (XMVector4IsNaN(input[x]) || XMVector4IsInfinite(input[x]))
				        {
					        throw Diagnostics::Error("Texture contains non-finite pixel data.");
				        }

				        const float red = XMVectorGetX(input[x]);
				        const float green = XMVectorGetY(input[x]);
				        const float blue = XMVectorGetZ(input[x]);
				        properties.bHasAlpha |= XMVectorGetW(input[x]) != 1.0f;
				        properties.bIsGray &= std::fabs(red - green) <= 1e-4f && std::fabs(red - blue) <= 1e-4f;
				        properties.bFitsUnsignedHalf &= std::max({red, green, blue}) <= 65504.0f;
			        }
		        }),
		    "inspect effective texture pixels");

		return properties;
	}

	DXGI_FORMAT SelectOutputFormat(const TextureCookRequest& request, bool bFloatValues, bool bNativeFloat, const PixelProperties& properties)
	{
		const auto uncompressed = bFloatValues ? DXGI_FORMAT_R32G32B32A32_FLOAT : DXGI_FORMAT_R8G8B8A8_UNORM;
		const auto hdr = request.policy.hdrEncoding == TextureHdrEncoding::Bc6h ? DXGI_FORMAT_BC6H_UF16 : DXGI_FORMAT_R32G32B32A32_FLOAT;
		DXGI_FORMAT format = uncompressed;
		switch (request.policy.textureGroup)
		{
			case TextureGroup::NormalMap:
				format = DXGI_FORMAT_BC5_UNORM;
				break;
			case TextureGroup::Roughness:
			case TextureGroup::Metallic:
			case TextureGroup::AmbientOcclusion:
			case TextureGroup::SubsurfaceStrength:
				format = properties.bIsGray ? DXGI_FORMAT_BC4_UNORM : uncompressed;
				break;
			case TextureGroup::Diffuse:
			case TextureGroup::Emissive:
			case TextureGroup::SubsurfaceColor:
				format = properties.bHasAlpha ? uncompressed : (bFloatValues ? hdr : DXGI_FORMAT_BC1_UNORM);
				break;
			case TextureGroup::HdrColor:
				format = properties.bHasAlpha ? DXGI_FORMAT_R32G32B32A32_FLOAT : hdr;
				break;
			case TextureGroup::Default:
				format = bNativeFloat && !properties.bHasAlpha ? hdr : uncompressed;
				break;
		}
		if (format == DXGI_FORMAT_BC6H_UF16 && !properties.bFitsUnsignedHalf)
		{
			throw Diagnostics::Error("Texture exceeds the finite unsigned-half range required by BC6H.");
		}
		return ApplyOutputIntent(format, request);
	}

	ScratchImage EncodeTexture(ScratchImage pixels, DXGI_FORMAT format, TextureMipPolicy policy)
	{
		if (format == DXGI_FORMAT_R32G32B32A32_FLOAT)
		{
			return pixels;
		}
		const auto images = SelectImages(pixels, policy);
		const auto metadata = SelectMetadata(pixels, policy);
		ScratchImage output;
		if (!IsCompressed(format))
		{
			CheckTextureOperation(Convert(images.data(), images.size(), metadata, format, TEX_FILTER_DEFAULT, TEX_THRESHOLD_DEFAULT, output), "encode output pixels");
			return output;
		}

		const auto flags = IsSRGB(format) ? TEX_COMPRESS_SRGB_OUT : TEX_COMPRESS_DEFAULT;
		CheckTextureOperation(Compress(images.data(), images.size(), metadata, format, flags, TEX_THRESHOLD_DEFAULT, output), "encode BC subresources");

		return output;
	}
}

using namespace TextureProcessing;

void CheckTextureOperation(HRESULT result, std::string_view operation)
{
	if (FAILED(result))
	{
		throw Diagnostics::Error(std::format("Texture {} failed (HRESULT {:#010x}).", operation, static_cast<std::uint32_t>(result)));
	}
}

bool CanPreserveTextureBlocks(const TextureCookRequest& request, DXGI_FORMAT format)
{
	const bool bSuppliedMips = request.policy.mipPolicy != TextureMipPolicy::Generate;
	const bool bUnchangedChannels = request.policy.channelMask == TextureChannelMask::Rgba;
	const bool bSignedNormal = request.policy.textureGroup == TextureGroup::NormalMap && format == DXGI_FORMAT_BC5_SNORM;
	const bool bDecodeHdr = format == DXGI_FORMAT_BC6H_UF16 && request.policy.hdrEncoding == TextureHdrEncoding::Rgba32Float;
	const bool bPreserveSemantics = !bSignedNormal && !bDecodeHdr;
	const bool bUnchangedEncoding = IsCompressed(format) && bPreserveSemantics;
	return bUnchangedEncoding && bSuppliedMips && bUnchangedChannels;
}

ScratchImage ProcessTexture(const TextureCookRequest& request, ScratchImage source)
{
	const auto sourceFormat = source.GetMetadata().format;
	if (CanPreserveTextureBlocks(request, sourceFormat))
	{
		source.OverrideFormat(ApplyOutputIntent(sourceFormat, request));
		return std::move(source);
	}

	const bool bNativeFloat = sourceFormat == DXGI_FORMAT_R32G32B32A32_FLOAT || sourceFormat == DXGI_FORMAT_BC6H_UF16;
	const bool bSignedValues = sourceFormat == DXGI_FORMAT_BC4_SNORM || sourceFormat == DXGI_FORMAT_BC5_SNORM;
	const bool bFloatValues = bNativeFloat || bSignedValues;
	ScratchImage pixels = DecodeWorkingPixels(source, request);
	if (bNativeFloat)
	{
		InspectPixels(pixels, request.policy.mipPolicy);
	}

	ApplySourcePolicy(pixels, request, sourceFormat, bFloatValues);
	GenerateMips(pixels, request);

	const auto policy = request.policy.mipPolicy == TextureMipPolicy::NoMips ? TextureMipPolicy::NoMips : TextureMipPolicy::PreserveExisting;
	const auto properties = InspectPixels(pixels, policy);
	const DXGI_FORMAT format = SelectOutputFormat(request, bFloatValues, bNativeFloat, properties);

	return EncodeTexture(std::move(pixels), format, policy);
}
