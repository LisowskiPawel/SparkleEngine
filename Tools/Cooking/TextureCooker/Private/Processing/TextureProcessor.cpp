#include "PCH.h"

#include "Processing/TextureProcessor.h"
#include "Core/Public/Diagnostics/Error.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <format>
#include <vector>

using namespace DirectX;

void CheckTextureCancellation(std::stop_token cancellation)
{
	if (cancellation.stop_requested())
	{
		throw Diagnostics::Error("Texture cooking was cancelled.");
	}
}

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

	void ApplySourcePolicy(ScratchImage& pixels, const TextureCookRequest& request, bool bWasFloat, std::stop_token cancellation)
	{
		const bool bDecodeSrgb = !bWasFloat && request.policy.colorProcessingPolicy == TextureColorProcessingPolicy::SrgbLinearize;
		const bool bExtractChannel = request.policy.channelMask != TextureChannelMask::Rgba;
		if (!bDecodeSrgb && !bExtractChannel)
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
		        [channel, bDecodeSrgb, bExtractChannel, cancellation](XMVECTOR* output, const XMVECTOR* input, std::size_t width, std::size_t)
		        {
			        CheckTextureCancellation(cancellation);
			        for (std::size_t x = 0; x < width; ++x)
			        {
				        const auto pixel = bDecodeSrgb ? XMColorSRGBToRGB(input[x]) : input[x];
				        output[x] = bExtractChannel ? XMVectorSetW(XMVectorReplicate(XMVectorGetByIndex(pixel, channel)), 1.0f) : pixel;
			        }
		        },
		        extracted),
		    "extract material channel");

		pixels = std::move(extracted);
	}

	void GenerateMips(ScratchImage& pixels, const TextureCookRequest& request, std::stop_token cancellation)
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
		CheckTextureCancellation(cancellation);
		if (group != TextureGroup::NormalMap)
		{
			return;
		}
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
				        [cancellation](XMVECTOR* output, const XMVECTOR* input, std::size_t width, std::size_t)
				        {
					        CheckTextureCancellation(cancellation);
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

	struct PixelProperties final
	{
		bool bHasAlpha = false;
		bool bIsGray = true;
		bool bFitsUnsignedHalf = true;
	};

	PixelProperties InspectPixels(const ScratchImage& pixels, TextureMipPolicy policy, std::stop_token cancellation)
	{
		PixelProperties properties;
		const auto images = SelectImages(pixels, policy);
		const auto metadata = SelectMetadata(pixels, policy);

		CheckTextureOperation(
		    EvaluateImage(
		        images.data(),
		        images.size(),
		        metadata,
		        [&properties, cancellation](const XMVECTOR* input, std::size_t width, std::size_t)
		        {
			        CheckTextureCancellation(cancellation);
			        for (std::size_t x = 0; x < width; ++x)
			        {
				        if (XMVector4IsNaN(input[x]) || XMVector4IsInfinite(input[x]))
				        {
					        throw Diagnostics::Error("Texture contains non-finite pixel data.");
				        }
				        const float red = XMVectorGetX(input[x]);
				        const float green = XMVectorGetY(input[x]);
				        const float blue = XMVectorGetZ(input[x]);
				        properties.bHasAlpha |= XMVectorGetW(input[x]) < 0.999f;
				        properties.bIsGray &= std::fabs(red - green) <= 1e-4f && std::fabs(red - blue) <= 1e-4f;
				        properties.bFitsUnsignedHalf &= std::max({red, green, blue}) <= 65504.0f;
			        }
		        }),
		    "inspect effective texture pixels");

		return properties;
	}

	DXGI_FORMAT SelectOutputFormat(const TextureCookRequest& request, bool bWasFloat, const PixelProperties& properties)
	{
		const auto uncompressed = bWasFloat ? DXGI_FORMAT_R32G32B32A32_FLOAT : DXGI_FORMAT_R8G8B8A8_UNORM;
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
				format = properties.bHasAlpha ? uncompressed : (bWasFloat ? DXGI_FORMAT_BC6H_UF16 : DXGI_FORMAT_BC1_UNORM);
				break;
			case TextureGroup::HdrColor:
				format = properties.bHasAlpha ? uncompressed : DXGI_FORMAT_BC6H_UF16;
				break;
			case TextureGroup::Default:
				break;
		}
		if (format == DXGI_FORMAT_BC6H_UF16 && !properties.bFitsUnsignedHalf)
		{
			throw Diagnostics::Error("Texture exceeds the finite unsigned-half range required by BC6H.");
		}
		return ApplyOutputIntent(format, request);
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
	return IsCompressed(format) && bSuppliedMips && bUnchangedChannels;
}

ScratchImage ProcessTexture(const TextureCookRequest& request, ScratchImage source, std::stop_token cancellation)
{
	CheckTextureCancellation(cancellation);
	const auto& sourceMetadata = source.GetMetadata();
	if (CanPreserveTextureBlocks(request, sourceMetadata.format))
	{
		source.OverrideFormat(ApplyOutputIntent(sourceMetadata.format, request));
		return std::move(source);
	}
	const bool bWasFloat = sourceMetadata.format == DXGI_FORMAT_R32G32B32A32_FLOAT || sourceMetadata.format == DXGI_FORMAT_BC6H_UF16;
	ScratchImage pixels = DecodeWorkingPixels(source, request);
	if (bWasFloat)
	{
		InspectPixels(pixels, request.policy.mipPolicy, cancellation);
	}
	ApplySourcePolicy(pixels, request, bWasFloat, cancellation);
	GenerateMips(pixels, request, cancellation);
	const auto policy = request.policy.mipPolicy == TextureMipPolicy::NoMips ? TextureMipPolicy::NoMips : TextureMipPolicy::PreserveExisting;
	const auto properties = InspectPixels(pixels, policy, cancellation);
	const DXGI_FORMAT format = SelectOutputFormat(request, bWasFloat, properties);
	if (format == DXGI_FORMAT_R32G32B32A32_FLOAT)
	{
		return pixels;
	}
	const auto images = SelectImages(pixels, policy);
	auto metadata = SelectMetadata(pixels, policy);
	ScratchImage output;
	if (!IsCompressed(format))
	{
		CheckTextureOperation(Convert(images.data(), images.size(), metadata, format, TEX_FILTER_DEFAULT, TEX_THRESHOLD_DEFAULT, output), "encode output pixels");
		return output;
	}
	metadata.format = format;
	CheckTextureOperation(output.Initialize(metadata), "allocate encoded subresources");
	CompressOptions options{};
	options.flags = request.IsSrgb() ? TEX_COMPRESS_SRGB_OUT : TEX_COMPRESS_DEFAULT;
	options.threshold = TEX_THRESHOLD_DEFAULT;
	options.alphaWeight = 1.0f;
	for (std::size_t index = 0; index < images.size(); ++index)
	{
		ScratchImage encoded;
		CheckTextureOperation(CompressEx(images[index], format, options, encoded, [cancellation](std::size_t, std::size_t) { return !cancellation.stop_requested(); }), "encode BC image");
		const Image& destination = output.GetImages()[index];
		const Image& actual = encoded.GetImages()[0];
		if (destination.rowPitch != actual.rowPitch || destination.slicePitch != actual.slicePitch)
		{
			throw Diagnostics::Error("Native BC output layout does not match its allocated subresource.");
		}
		std::memcpy(destination.pixels, actual.pixels, actual.slicePitch);
	}
	return output;
}
