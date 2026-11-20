#include "PCH.h"

#include "Processing/TextureProcessor.h"
#include "Core/Public/Diagnostics/Error.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <format>
#include <span>
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
			const auto result = Decompress(images.data(), images.size(), metadata, DXGI_FORMAT_R32G32B32A32_FLOAT, pixels);
			CheckTextureOperation(result, "decompress source");
		}
		else
		{
			const auto result = Convert(images.data(), images.size(), metadata, DXGI_FORMAT_R32G32B32A32_FLOAT, TEX_FILTER_FORCE_NON_WIC, TEX_THRESHOLD_DEFAULT, pixels);
			CheckTextureOperation(result, "convert source");
		}
		source.Release();
		return pixels;
	}

	XMVECTOR DecodeBc5Normal(XMVECTOR pixel, bool bSigned)
	{
		auto xy = pixel;
		if (!bSigned)
		{
			const auto scaled = XMVectorScale(pixel, 2.0f);
			xy = XMVectorSubtract(scaled, g_XMOne);
		}
		const auto xyLengthSquared = XMVector2LengthSq(xy);
		const auto zSquared = XMVectorSubtract(g_XMOne, xyLengthSquared);
		const auto clampedZSquared = XMVectorMax(zSquared, g_XMZero);
		const auto z = XMVectorSqrt(clampedZSquared);
		const auto normal = XMVectorSetZ(xy, XMVectorGetX(z));
		const auto encodedNormal = XMVectorMultiplyAdd(normal, g_XMOneHalf, g_XMOneHalf);
		return XMVectorSetW(encodedNormal, XMVectorGetW(pixel));
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

		const auto transformResult = TransformImage(
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
				    if (bExtractChannel)
				    {
					    const auto selectedChannel = XMVectorGetByIndex(pixel, channel);
					    const auto grayscale = XMVectorReplicate(selectedChannel);
					    pixel = XMVectorSetW(grayscale, 1.0f);
				    }
				    output[x] = pixel;
			    }
		    },
		    extracted);

		CheckTextureOperation(transformResult, "apply source pixel semantics");
		pixels = std::move(extracted);
	}

	void NormalizeNormals(ScratchImage& pixels, TextureMipPolicy policy)
	{
		const auto& metadata = pixels.GetMetadata();
		const auto levels = policy == TextureMipPolicy::NoMips ? 1 : metadata.mipLevels;
		for (std::size_t item = 0; item < metadata.arraySize; ++item)
		{
			for (std::size_t mip = 0; mip < levels; ++mip)
			{
				const Image& image = *pixels.GetImage(mip, item, 0);
				ScratchImage normalized;

				const auto normalizeResult = TransformImage(
				    image,
				    [](XMVECTOR* output, const XMVECTOR* input, std::size_t width, std::size_t)
				    {
					    for (std::size_t x = 0; x < width; ++x)
					    {
						    const auto scaled = XMVectorScale(input[x], 2.0f);
						    auto normal = XMVectorSubtract(scaled, XMVectorReplicate(1.0f));
						    normal = XMVector3Normalize(normal);
						    const auto biased = XMVectorAdd(normal, XMVectorReplicate(1.0f));
						    const auto encoded = XMVectorScale(biased, 0.5f);
						    const auto alphaMask = XMVectorSelectControl(0, 0, 0, 1);
						    output[x] = XMVectorSelect(encoded, input[x], alphaMask);
					    }
				    },
				    normalized);

				CheckTextureOperation(normalizeResult, "normalize tangent-space normals");
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
		const auto result = GenerateMipMaps(pixels.GetImages(), pixels.GetImageCount(), pixels.GetMetadata(), flags, 0, generated);
		CheckTextureOperation(result, "generate mip chain");
		pixels = std::move(generated);
	}

	struct PixelProperties final
	{
		bool bHasAlpha = false;
		bool bIsGray = true;
		bool bFitsUnsignedHalf = true;
		bool bHasNegativeNormalZ = false;
	};

	PixelProperties InspectPixels(const ScratchImage& pixels, TextureMipPolicy policy)
	{
		PixelProperties properties;
		const auto images = SelectImages(pixels, policy);
		const auto metadata = SelectMetadata(pixels, policy);

		const auto inspectionResult = EvaluateImage(
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
				    properties.bHasNegativeNormalZ |= blue < 0.5f;
			    }
		    });

		CheckTextureOperation(inspectionResult, "inspect effective texture pixels");
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
				format = properties.bHasNegativeNormalZ || request.IsSrgb() ? DXGI_FORMAT_BC7_UNORM : DXGI_FORMAT_BC5_UNORM;
				break;
			case TextureGroup::Roughness:
			case TextureGroup::Metallic:
			case TextureGroup::AmbientOcclusion:
			case TextureGroup::SubsurfaceStrength:
				format = properties.bIsGray ? (bFloatValues ? DXGI_FORMAT_R32_FLOAT : DXGI_FORMAT_BC4_UNORM) : uncompressed;
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

	void ValidateBc6hQuality(std::span<const Image> reference, const ScratchImage& encoded)
	{
		ScratchImage decoded;
		const auto decodeResult = Decompress(encoded.GetImages(), encoded.GetImageCount(), encoded.GetMetadata(), DXGI_FORMAT_R32G32B32A32_FLOAT, decoded);
		CheckTextureOperation(decodeResult, "verify BC6H pixels");
		for (std::size_t index = 0; index < reference.size(); ++index)
		{
			const auto& image = reference[index];
			double squaredError = 0;
			bool bExceedsLimit = false;
			const auto evaluationResult = EvaluateImage(
			    decoded.GetImages()[index],
			    [&image, &squaredError, &bExceedsLimit](const XMVECTOR* pixels, std::size_t width, std::size_t y)
			    {
				    const auto* row = reinterpret_cast<const XMFLOAT4*>(image.pixels + y * image.rowPitch);
				    for (std::size_t x = 0; x < width; ++x)
				    {
					    const auto expected = XMVectorMax(XMLoadFloat4(&row[x]), g_XMZero);
					    for (std::size_t channel = 0; channel < 3; ++channel)
					    {
						    const double value = XMVectorGetByIndex(expected, channel);
						    const double decodedValue = XMVectorGetByIndex(pixels[x], channel);
						    const double error = std::fabs(decodedValue - value);
						    const double limit = value > 1.0 ? value * 0.15 : 0.08;
						    bExceedsLimit |= !(error <= limit);
						    const double relative = error / std::max(value, 1.0);
						    squaredError += relative * relative;
					    }
				    }
			    });

			CheckTextureOperation(evaluationResult, "evaluate BC6H quality");
			const double maximumSquaredError = 0.04 * 0.04 * image.width * image.height * 3;
			if (bExceedsLimit || squaredError > maximumSquaredError)
			{
				throw Diagnostics::Error(std::format("BC6H quality limit exceeded at subresource {}. Use rgba32f.", index));
			}
		}
	}

	ScratchImage EncodeTexture(ScratchImage pixels, DXGI_FORMAT format, TextureMipPolicy policy, TextureGroup group)
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
			const auto result = Convert(images.data(), images.size(), metadata, format, TEX_FILTER_FORCE_NON_WIC, TEX_THRESHOLD_DEFAULT, output);
			CheckTextureOperation(result, "encode output pixels");
			return output;
		}

		auto flags = IsSRGB(format) ? TEX_COMPRESS_SRGB_OUT : TEX_COMPRESS_DEFAULT;
		if (group == TextureGroup::NormalMap)
		{
			flags = static_cast<TEX_COMPRESS_FLAGS>(flags | TEX_COMPRESS_UNIFORM);
		}
		const auto result = Compress(images.data(), images.size(), metadata, format, flags, TEX_THRESHOLD_DEFAULT, output);
		CheckTextureOperation(result, "encode BC subresources");
		if (format == DXGI_FORMAT_BC6H_UF16)
		{
			ValidateBc6hQuality(images, output);
		}

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
	const bool bSignedValues = format == DXGI_FORMAT_BC4_SNORM || format == DXGI_FORMAT_BC5_SNORM;
	const bool bLinearSource = format == DXGI_FORMAT_BC6H_UF16 || bSignedValues;
	const bool bDecodeSrgb = !bLinearSource && request.policy.colorProcessingPolicy == TextureColorProcessingPolicy::SrgbLinearize;
	const bool bUnchangedTransfer = bDecodeSrgb == request.IsSrgb();
	const bool bOutputIntentSupported = !request.IsSrgb() || IsSRGB(MakeSRGB(format));
	const bool bPreserveSemantics = !bSignedNormal && !bDecodeHdr;
	const bool bUnchangedEncoding = IsCompressed(format) && bPreserveSemantics;
	return bUnchangedEncoding && bSuppliedMips && bUnchangedChannels && bUnchangedTransfer && bOutputIntentSupported;
}

ScratchImage ProcessTexture(const TextureCookRequest& request, ScratchImage source)
{
	const auto sourceFormat = source.GetMetadata().format;
	if (CanPreserveTextureBlocks(request, sourceFormat))
	{
		source.OverrideFormat(ApplyOutputIntent(sourceFormat, request));
		return std::move(source);
	}

	const bool bUncompressedFloat = sourceFormat == DXGI_FORMAT_R32G32B32A32_FLOAT || sourceFormat == DXGI_FORMAT_R16G16B16A16_FLOAT;
	const bool bNativeFloat = bUncompressedFloat || sourceFormat == DXGI_FORMAT_BC6H_UF16;
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
	if (request.policy.textureGroup == TextureGroup::NormalMap)
	{
		NormalizeNormals(pixels, policy);
	}
	const auto properties = InspectPixels(pixels, policy);
	const DXGI_FORMAT format = SelectOutputFormat(request, bFloatValues, bNativeFloat, properties);

	return EncodeTexture(std::move(pixels), format, policy, request.policy.textureGroup);
}
