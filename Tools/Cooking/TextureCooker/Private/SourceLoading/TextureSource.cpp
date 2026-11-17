#include "PCH.h"

#include "SourceLoading/TextureSource.h"
#include "TextureCookRequestList.h"
#include "SourceAssetPaths.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Paths/PathUtils.h"

#define STBI_ONLY_PSD
#define STBI_ONLY_PIC
#define STBI_ONLY_PNM
#define STBI_ONLY_TGA
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define TINYEXR_USE_MINIZ 0
#define TINYEXR_IMPLEMENTATION
#include <zlib.h>
#include <tinyexr.h>

#include <algorithm>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <memory>
#include <vector>

using namespace DirectX;

namespace TextureSourceLoading
{
	std::vector<std::uint8_t> ReadSource(const std::filesystem::path& path)
	{
		const auto resolved = SparkleTools::ResolveSourceAssetPath(path, SparkleTools::SourceAssetType::Texture);
		if (!resolved)
		{
			throw Diagnostics::Error("Unable to resolve source texture.");
		}
		std::ifstream file(*resolved, std::ios::binary | std::ios::ate);
		const auto size = file.tellg();
		if (size <= 0 || size > static_cast<std::streamoff>(MaximumTextureSourceBytes))
		{
			throw Diagnostics::Error("Source texture is unreadable, empty or exceeds the 64 MiB file limit.");
		}
		std::vector<std::uint8_t> bytes(static_cast<std::size_t>(size));
		file.seekg(0);
		file.read(reinterpret_cast<char*>(bytes.data()), size);
		if (!file || file.peek() != std::ifstream::traits_type::eof())
		{
			throw Diagnostics::Error("Source texture changed size or could not be read completely.");
		}
		return bytes;
	}

	TexMetadata FlatMetadata(int width, int height, DXGI_FORMAT format)
	{
		TexMetadata metadata{};
		metadata.width = width > 0 ? static_cast<std::size_t>(width) : 0;
		metadata.height = height > 0 ? static_cast<std::size_t>(height) : 0;
		metadata.depth = metadata.arraySize = metadata.mipLevels = 1;
		metadata.dimension = TEX_DIMENSION_TEXTURE2D;
		metadata.format = format;
		return metadata;
	}

	TextureSource LoadProviderPixels(std::span<const std::uint8_t> bytes, bool bExr, const TextureCookRequest& request, TextureCookMemoryLimiter& processingMemory, std::stop_token cancellation)
	{
		TextureSource source;
		int width = 0;
		int height = 0;
		if (!bExr)
		{
			int channels = 0;
			if (!stbi_info_from_memory(bytes.data(), static_cast<int>(bytes.size()), &width, &height, &channels))
			{
				throw Diagnostics::Error("Unable to read raster source metadata.");
			}
			const auto metadata = FlatMetadata(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
			source.ProcessingMemory = processingMemory.Acquire(MeasureTextureProcessingMemory(metadata, bytes.size(), request), cancellation);

			std::unique_ptr<stbi_uc, decltype(&stbi_image_free)> pixels(
			    stbi_load_from_memory(bytes.data(), static_cast<int>(bytes.size()), &width, &height, &channels, STBI_rgb_alpha),
			    &stbi_image_free);

			if (!pixels || metadata.width != width || metadata.height != height)
			{
				throw Diagnostics::Error("Raster source decode failed or disagreed with admitted metadata.");
			}
			CheckTextureOperation(source.Images.Initialize(metadata), "allocate imported raster");
			std::memcpy(source.Images.GetPixels(), pixels.get(), source.Images.GetPixelsSize());
			return source;
		}
		EXRVersion version{};
		const int versionResult = ParseEXRVersionFromMemory(&version, bytes.data(), bytes.size());
		const bool bImagePart = !version.multipart && !version.non_image;
		if (versionResult != TINYEXR_SUCCESS || !bImagePart)
		{
			throw Diagnostics::Error("EXR source must be a flat single-part image.");
		}
		EXRHeader header;
		InitEXRHeader(&header);

		auto freeHeader = [](EXRHeader* value)
		{
			FreeEXRHeader(value);
		};

		const std::unique_ptr<EXRHeader, decltype(freeHeader)> headerScope(&header, freeHeader);
		const char* error = nullptr;
		const int parsed = ParseEXRHeaderFromMemory(&header, &version, bytes.data(), bytes.size(), &error);
		if (error)
		{
			FreeEXRErrorMessage(error);
			error = nullptr;
		}
		if (parsed != TINYEXR_SUCCESS || header.num_channels < 1 || header.num_channels > 4)
		{
			throw Diagnostics::Error("Unable to read EXR source metadata.");
		}
		const std::int64_t admittedWidth = static_cast<std::int64_t>(header.data_window.max_x) - header.data_window.min_x + 1;
		const std::int64_t admittedHeight = static_cast<std::int64_t>(header.data_window.max_y) - header.data_window.min_y + 1;
		if (admittedWidth <= 0 || admittedWidth > 16384 || admittedHeight <= 0 || admittedHeight > 16384)
		{
			throw Diagnostics::Error("EXR source dimensions exceed the supported 2D scope.");
		}
		const auto metadata = FlatMetadata(static_cast<int>(admittedWidth), static_cast<int>(admittedHeight), DXGI_FORMAT_R32G32B32A32_FLOAT);
		source.ProcessingMemory = processingMemory.Acquire(MeasureTextureProcessingMemory(metadata, bytes.size(), request), cancellation);
		float* decoded = nullptr;
		const int loaded = LoadEXRFromMemory(&decoded, &width, &height, bytes.data(), bytes.size(), &error);
		std::unique_ptr<float, decltype(&std::free)> pixels(decoded, &std::free);
		if (error)
		{
			FreeEXRErrorMessage(error);
		}
		if (loaded != TINYEXR_SUCCESS || !pixels || width != admittedWidth || height != admittedHeight)
		{
			throw Diagnostics::Error("EXR source decode failed or disagreed with admitted metadata.");
		}
		CheckTextureOperation(source.Images.Initialize(metadata), "allocate imported EXR");
		std::memcpy(source.Images.GetPixels(), pixels.get(), source.Images.GetPixelsSize());
		return source;
	}
}

using namespace TextureSourceLoading;

std::size_t MeasureTextureProcessingMemory(const TexMetadata& metadata, std::size_t fileBytes, const TextureCookRequest& request)
{
	const bool bDimensionsInRange = metadata.width > 0 && metadata.width <= 16384;
	const bool bHeightInRange = metadata.height > 0 && metadata.height <= 16384;
	const bool bLayersInRange = metadata.arraySize > 0 && metadata.arraySize <= 2048;
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
	const bool bByteRgba = metadata.format == DXGI_FORMAT_R8G8B8A8_UNORM || metadata.format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	const bool bByteBgra = metadata.format == DXGI_FORMAT_B8G8R8A8_UNORM || metadata.format == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	const bool bBasicBc = metadata.format >= DXGI_FORMAT_BC1_UNORM && metadata.format <= DXGI_FORMAT_BC5_SNORM;
	const bool bBc7 = metadata.format == DXGI_FORMAT_BC7_UNORM || metadata.format == DXGI_FORMAT_BC7_UNORM_SRGB;
	const bool bModernBc = metadata.format == DXGI_FORMAT_BC6H_UF16 || bBc7;
	const bool bLdrRgba = bByteRgba || metadata.format == DXGI_FORMAT_R16G16B16A16_UNORM;
	const bool bLdrColor = bLdrRgba || bByteBgra;
	const bool bBlocks = bBasicBc || bModernBc;
	const bool bScalar = metadata.format == DXGI_FORMAT_R8_UNORM || metadata.format == DXGI_FORMAT_R16_UNORM;
	const bool bIntegerPixels = bLdrColor || bScalar;
	const bool bUncompressed = bIntegerPixels || metadata.format == DXGI_FORMAT_R32G32B32A32_FLOAT;
	const bool bSupported = bUncompressed || bBlocks;
	if (!bSupported || IsTypeless(metadata.format))
	{
		throw Diagnostics::Error("Unsupported source texture encoding.");
	}
	std::uint64_t texels = 0;
	std::uint64_t nativeBytes = 0;
	std::size_t width = metadata.width;
	std::size_t height = metadata.height;
	for (std::size_t mip = 0; mip < metadata.mipLevels; ++mip)
	{
		texels += static_cast<std::uint64_t>(width) * height * metadata.arraySize;
		std::size_t row = 0;
		std::size_t slice = 0;
		CheckTextureOperation(ComputePitch(metadata.format, width, height, row, slice), "measure source subresource");
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

TextureSource LoadTextureSource(const TextureCookRequest& request, TextureCookMemoryLimiter& processingMemory, std::stop_token cancellation)
{
	const auto bytes = ReadSource(request.sourcePath);
	const auto extension = Paths::GetLowercaseExtension(request.sourcePath);
	if (extension == L".ktx" || extension == L".ktx2")
	{
		return LoadKtxTextureSource(bytes, request, processingMemory, cancellation);
	}
	if (extension == L".exr")
	{
		return LoadProviderPixels(bytes, true, request, processingMemory, cancellation);
	}
	if (extension == L".psd" || extension == L".pic" || extension == L".pnm" || extension == L".ppm" || extension == L".pgm" || extension == L".tga")
	{
		return LoadProviderPixels(bytes, false, request, processingMemory, cancellation);
	}
	TextureSource source;
	TexMetadata metadata{};
	constexpr auto wicFlags = static_cast<WIC_FLAGS>(WIC_FLAGS_FORCE_RGB | WIC_FLAGS_NO_16BPP | WIC_FLAGS_IGNORE_SRGB);
	HRESULT result;
	if (extension == L".dds")
	{
		result = GetMetadataFromDDSMemory(bytes.data(), bytes.size(), DDS_FLAGS_NONE, metadata);
	}
	else if (extension == L".hdr" || extension == L".hdri")
	{
		result = GetMetadataFromHDRMemory(bytes.data(), bytes.size(), metadata);
	}
	else if (extension == L".png" || extension == L".jpg" || extension == L".jpeg" || extension == L".bmp" || extension == L".gif")
	{
		result = GetMetadataFromWICMemory(bytes.data(), bytes.size(), static_cast<WIC_FLAGS>(wicFlags | WIC_FLAGS_ALL_FRAMES), metadata);
		if (SUCCEEDED(result) && metadata.arraySize != 1)
		{
			throw Diagnostics::Error("Animated or multi-frame raster sources are unsupported.");
		}
	}
	else
	{
		throw Diagnostics::Error("Unsupported source texture extension.");
	}
	CheckTextureOperation(result, "read source metadata");
	source.ProcessingMemory = processingMemory.Acquire(MeasureTextureProcessingMemory(metadata, bytes.size(), request), cancellation);
	if (extension == L".dds")
	{
		result = LoadFromDDSMemory(bytes.data(), bytes.size(), DDS_FLAGS_NONE, nullptr, source.Images);
	}
	else if (extension == L".hdr" || extension == L".hdri")
	{
		result = LoadFromHDRMemory(bytes.data(), bytes.size(), nullptr, source.Images);
	}
	else
	{
		result = LoadFromWICMemory(bytes.data(), bytes.size(), wicFlags, nullptr, source.Images);
	}
	CheckTextureOperation(result, "decode source");
	const auto& loaded = source.Images.GetMetadata();
	if (metadata.width != loaded.width || metadata.height != loaded.height || metadata.arraySize != loaded.arraySize || metadata.mipLevels != loaded.mipLevels || metadata.format != loaded.format)
	{
		throw Diagnostics::Error("Native source decode disagreed with admitted metadata.");
	}
	return source;
}
