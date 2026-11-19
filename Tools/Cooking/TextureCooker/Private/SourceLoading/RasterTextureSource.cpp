#include "PCH.h"

#include "SourceLoading/TextureSource.h"
#include "Processing/TextureProcessor.h"
#include "Core/Public/Diagnostics/Error.h"

#define STBI_ONLY_PNG
#define STBI_ONLY_PSD
#define STBI_ONLY_PIC
#define STBI_ONLY_PNM
#define STBI_ONLY_TGA
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cstring>
#include <memory>

using namespace DirectX;

TextureSource LoadRasterTextureSource(std::span<const std::uint8_t> bytes, const TextureCookRequest& request, TextureCookMemoryLimiter& processingMemory)
{
	TextureSource source;
	int width = 0;
	int height = 0;
	int channels = 0;
	if (!stbi_info_from_memory(bytes.data(), static_cast<int>(bytes.size()), &width, &height, &channels))
	{
		throw Diagnostics::Error("Unable to read raster source metadata.");
	}

	const bool b16Bit = stbi_is_16_bit_from_memory(bytes.data(), static_cast<int>(bytes.size())) != 0;
	const auto format = b16Bit ? DXGI_FORMAT_R16G16B16A16_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM;
	const auto metadata = MakeFlatTextureMetadata(width, height, format);
	source.ProcessingMemory = processingMemory.Acquire(MeasureTextureProcessingMemory(metadata, bytes.size(), request));

	std::unique_ptr<void, decltype(&stbi_image_free)> pixels(
	    b16Bit ? static_cast<void*>(stbi_load_16_from_memory(bytes.data(), static_cast<int>(bytes.size()), &width, &height, &channels, STBI_rgb_alpha))
	           : stbi_load_from_memory(bytes.data(), static_cast<int>(bytes.size()), &width, &height, &channels, STBI_rgb_alpha),
	    &stbi_image_free);

	if (!pixels || metadata.width != width || metadata.height != height)
	{
		throw Diagnostics::Error("Raster source decode failed or disagreed with admitted metadata.");
	}

	CheckTextureOperation(source.Images.Initialize(metadata), "allocate imported raster");
	std::memcpy(source.Images.GetPixels(), pixels.get(), source.Images.GetPixelsSize());
	return source;
}
