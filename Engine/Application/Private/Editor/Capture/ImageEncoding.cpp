#include "PCH.h"

#include "Editor/Capture/ImageEncoding.h"

#include <DirectXPackedVector.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <memory>
#include <utility>

#include <tinyexr.h>

enum class ImagePixelEncoding : std::uint8_t
{
	Rgba32Float,
	Rgba16Float,
	Rgba8Unorm,
	Bgra8Unorm,
};

#pragma pack(push, 1)

struct BmpFileHeader final
{
	std::uint16_t Type = 0x4D42;
	std::uint32_t Size = 0;
	std::uint16_t Reserved1 = 0;
	std::uint16_t Reserved2 = 0;
	std::uint32_t OffBits = 54;
};

struct BmpInfoHeader final
{
	std::uint32_t Size = sizeof(BmpInfoHeader);
	std::int32_t Width = 0;
	std::int32_t Height = 0;
	std::uint16_t Planes = 1;
	std::uint16_t BitCount = 32;
	std::uint32_t Compression = 0;
	std::uint32_t SizeImage = 0;
	std::int32_t XPelsPerMeter = 2835;
	std::int32_t YPelsPerMeter = 2835;
	std::uint32_t ClrUsed = 0;
	std::uint32_t ClrImportant = 0;
};

#pragma pack(pop)

static bool ResolvePixelFormat(PixelFormat format, ImagePixelEncoding& imageFormat) noexcept
{
	switch (format)
	{
		case PixelFormat::R32G32B32A32_Float:
			imageFormat = ImagePixelEncoding::Rgba32Float;
			return true;

		case PixelFormat::R16G16B16A16_Float:
			imageFormat = ImagePixelEncoding::Rgba16Float;
			return true;

		case PixelFormat::R8G8B8A8_UNorm:
		case PixelFormat::R8G8B8A8_UNorm_Srgb:
			imageFormat = ImagePixelEncoding::Rgba8Unorm;
			return true;

		case PixelFormat::B8G8R8A8_UNorm:
		case PixelFormat::B8G8R8A8_UNorm_Srgb:
			imageFormat = ImagePixelEncoding::Bgra8Unorm;
			return true;
		default:
			return false;
	}
}

static std::byte ToByte(float value) noexcept
{
	const float clamped = std::clamp(value, 0.0f, 1.0f);
	const auto rounded = static_cast<std::uint32_t>(clamped * 255.0f + 0.5f);
	return static_cast<std::byte>(rounded);
}

static void ConvertPixel(const std::byte* source, ImagePixelEncoding encoding, std::byte* destination) noexcept
{
	switch (encoding)
	{
		case ImagePixelEncoding::Rgba32Float:
		{
			std::array<float, 4> rgba{};
			std::memcpy(rgba.data(), source, sizeof(rgba));
			destination[0] = ToByte(rgba[2]);
			destination[1] = ToByte(rgba[1]);
			destination[2] = ToByte(rgba[0]);
			destination[3] = ToByte(rgba[3]);
			break;
		}
		case ImagePixelEncoding::Rgba16Float:
		{
			std::array<std::uint16_t, 4> rgba{};
			std::memcpy(rgba.data(), source, sizeof(rgba));
			destination[0] = ToByte(DirectX::PackedVector::XMConvertHalfToFloat(rgba[2]));
			destination[1] = ToByte(DirectX::PackedVector::XMConvertHalfToFloat(rgba[1]));
			destination[2] = ToByte(DirectX::PackedVector::XMConvertHalfToFloat(rgba[0]));
			destination[3] = ToByte(DirectX::PackedVector::XMConvertHalfToFloat(rgba[3]));
			break;
		}

		case ImagePixelEncoding::Rgba8Unorm:
			destination[0] = source[2];

			destination[1] = source[1];
			destination[2] = source[0];
			destination[3] = source[3];
			break;
		case ImagePixelEncoding::Bgra8Unorm:
			std::memcpy(destination, source, 4u);
			break;
	}
}

bool ImageEncoding::DecodeLinearRgb(const ImageBufferView& source, LinearRgbImage& image, std::string& errorMessage)
{
	constexpr std::uint32_t bytesPerPixel = sizeof(float) * 4u;
	const std::uint64_t minimumRowPitch = static_cast<std::uint64_t>(source.Width) * bytesPerPixel;
	const std::uint64_t requiredBytes = static_cast<std::uint64_t>(source.RowPitch) * source.Height;
	const bool bEmptyExtent = source.Width == 0u || source.Height == 0u;
	const bool bIncompleteStorage = source.RowPitch < minimumRowPitch || source.Pixels.size() < requiredBytes;
	const bool bInvalidLayout = bEmptyExtent || bIncompleteStorage;

	if (source.Format != PixelFormat::R32G32B32A32_Float || bInvalidLayout)
	{
		errorMessage = "Image is not a complete RGBA32_FLOAT buffer.";
		return false;
	}

	image.Width = source.Width;
	image.Height = source.Height;
	image.Pixels.resize(static_cast<std::size_t>(image.Width) * image.Height * 3u);
	for (std::uint32_t y = 0; y < image.Height; ++y)
	{
		const std::byte* sourceRow = source.Pixels.data() + static_cast<std::size_t>(source.RowPitch) * y;
		for (std::uint32_t x = 0; x < image.Width; ++x)
		{
			std::array<float, 4> sourcePixel{};
			std::memcpy(sourcePixel.data(), sourceRow + static_cast<std::size_t>(x) * bytesPerPixel, bytesPerPixel);
			const std::size_t destination = (static_cast<std::size_t>(y) * image.Width + x) * 3u;
			for (std::size_t channel = 0; channel < 3u; ++channel)
			{
				if (!std::isfinite(sourcePixel[channel]))
				{
					errorMessage = "Image contains a non-finite color value.";
					return false;
				}
				image.Pixels[destination + channel] = sourcePixel[channel];
			}
		}
	}
	errorMessage.clear();
	return true;
}

bool ImageEncoding::EncodeBmp(const ImageBufferView& image, std::vector<std::byte>& encodedBytes, std::string& errorMessage)
{
	ImagePixelEncoding imageFormat = ImagePixelEncoding::Rgba8Unorm;
	const std::uint32_t bytesPerPixel = PixelFormatBytesPerTexel(image.Format);
	const std::uint64_t minimumRowPitch = static_cast<std::uint64_t>(image.Width) * bytesPerPixel;
	const std::uint64_t requiredBytes = static_cast<std::uint64_t>(image.RowPitch) * image.Height;
	const bool bPixelFormatResolved = ResolvePixelFormat(image.Format, imageFormat);
	const bool bEmptyExtent = image.Width == 0u || image.Height == 0u;
	const bool bIncompleteStorage = image.RowPitch < minimumRowPitch || image.Pixels.size() < requiredBytes;
	const bool bInvalidLayout = bEmptyExtent || bIncompleteStorage;

	if (!bPixelFormatResolved || bInvalidLayout)
	{
		errorMessage = "Image cannot be encoded as BMP.";
		return false;
	}

	BmpFileHeader fileHeader;
	BmpInfoHeader infoHeader;
	const std::size_t headerBytes = sizeof(fileHeader) + sizeof(infoHeader);
	const std::size_t outputRowPitch = static_cast<std::size_t>(image.Width) * 4u;
	const std::size_t maximumPixelBytes = (std::numeric_limits<std::uint32_t>::max)() - headerBytes;
	if (outputRowPitch > maximumPixelBytes / image.Height)
	{
		errorMessage = "Image storage exceeds the BMP format limit.";
		return false;
	}

	const std::size_t outputPixelBytes = outputRowPitch * image.Height;
	std::vector<std::byte> output(headerBytes + outputPixelBytes);
	std::byte* const outputPixels = output.data() + headerBytes;
	for (std::uint32_t y = 0; y < image.Height; ++y)
	{
		const std::byte* sourceRow = image.Pixels.data() + static_cast<std::size_t>(image.RowPitch) * y;
		std::byte* outputRow = outputPixels + outputRowPitch * y;
		for (std::uint32_t x = 0; x < image.Width; ++x)
		{
			const std::byte* sourcePixel = sourceRow + static_cast<std::size_t>(x) * bytesPerPixel;
			std::byte* outputPixel = outputRow + static_cast<std::size_t>(x) * 4u;
			ConvertPixel(sourcePixel, imageFormat, outputPixel);
		}
	}

	infoHeader.Width = static_cast<std::int32_t>(image.Width);
	infoHeader.Height = -static_cast<std::int32_t>(image.Height);
	infoHeader.SizeImage = static_cast<std::uint32_t>(outputPixelBytes);
	fileHeader.Size = fileHeader.OffBits + infoHeader.SizeImage;
	std::byte* destination = output.data();
	std::memcpy(destination, &fileHeader, sizeof(fileHeader));
	destination += sizeof(fileHeader);
	std::memcpy(destination, &infoHeader, sizeof(infoHeader));
	encodedBytes = std::move(output);
	errorMessage.clear();
	return true;
}

bool ImageEncoding::EncodeExr(const LinearRgbImage& image, std::vector<std::byte>& encodedBytes, std::string& errorMessage)
{
	const std::size_t pixelCount = static_cast<std::size_t>(image.Width) * image.Height;
	std::array<std::vector<float>, 3> channels;
	for (std::vector<float>& channel : channels)
	{
		channel.resize(pixelCount);
	}
	for (std::size_t pixel = 0; pixel < pixelCount; ++pixel)
	{
		channels[0][pixel] = image.Pixels[pixel * 3u + 2u];
		channels[1][pixel] = image.Pixels[pixel * 3u + 1u];
		channels[2][pixel] = image.Pixels[pixel * 3u + 0u];
	}

	EXRImage exrImage;
	InitEXRImage(&exrImage);
	exrImage.num_channels = 3;
	exrImage.width = static_cast<int>(image.Width);
	exrImage.height = static_cast<int>(image.Height);

	std::array<unsigned char*, 3> imagePointers = {
	    reinterpret_cast<unsigned char*>(channels[0].data()),
	    reinterpret_cast<unsigned char*>(channels[1].data()),
	    reinterpret_cast<unsigned char*>(channels[2].data())};

	exrImage.images = imagePointers.data();

	EXRHeader header;
	InitEXRHeader(&header);
	header.num_channels = 3;
	std::array<EXRChannelInfo, 3> channelInfo{};
	std::strncpy(channelInfo[0].name, "B", 255);
	std::strncpy(channelInfo[1].name, "G", 255);
	std::strncpy(channelInfo[2].name, "R", 255);
	header.channels = channelInfo.data();
	std::array<int, 3> pixelTypes = {TINYEXR_PIXELTYPE_FLOAT, TINYEXR_PIXELTYPE_FLOAT, TINYEXR_PIXELTYPE_FLOAT};
	std::array<int, 3> requestedPixelTypes = pixelTypes;
	header.pixel_types = pixelTypes.data();
	header.requested_pixel_types = requestedPixelTypes.data();
	header.compression_type = TINYEXR_COMPRESSIONTYPE_ZIP;

	unsigned char* encoded = nullptr;
	const char* exrError = nullptr;
	const std::size_t encodedSize = SaveEXRImageToMemory(&exrImage, &header, &encoded, &exrError);
	const std::unique_ptr<unsigned char, decltype(&std::free)> encodedScope(encoded, &std::free);
	const std::unique_ptr<const char, decltype(&FreeEXRErrorMessage)> errorScope(exrError, &FreeEXRErrorMessage);
	if (encodedSize == 0u || !encodedScope)
	{
		errorMessage = errorScope ? errorScope.get() : "TinyEXR could not encode the image.";
		return false;
	}
	encodedBytes.resize(encodedSize);
	std::memcpy(encodedBytes.data(), encodedScope.get(), encodedSize);
	errorMessage.clear();
	return true;
}
