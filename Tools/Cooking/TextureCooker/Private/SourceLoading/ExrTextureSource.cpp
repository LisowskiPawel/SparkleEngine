#include "PCH.h"

#include "SourceLoading/TextureSource.h"
#include "Processing/TextureProcessor.h"
#include "Core/Public/Diagnostics/Error.h"

#define TINYEXR_USE_MINIZ 0
#define TINYEXR_IMPLEMENTATION
#include <zlib.h>
#include <tinyexr.h>

#include <cstdlib>
#include <cstring>
#include <memory>

using namespace DirectX;

namespace ExrSourceLoading
{
	TexMetadata ReadMetadata(std::span<const std::uint8_t> bytes)
	{
		EXRVersion version{};
		const int versionResult = ParseEXRVersionFromMemory(&version, bytes.data(), bytes.size());
		const bool bImagePart = !version.multipart && !version.non_image;
		if (versionResult != TINYEXR_SUCCESS || !bImagePart)
		{
			throw Diagnostics::Error("EXR source must be a flat single-part image.");
		}
		EXRHeader header;
		InitEXRHeader(&header);

		const std::unique_ptr<EXRHeader, decltype(&FreeEXRHeader)> headerScope(&header, &FreeEXRHeader);
		const int parsed = ParseEXRHeaderFromMemory(&header, &version, bytes.data(), bytes.size(), nullptr);
		if (parsed != TINYEXR_SUCCESS || header.num_channels < 1 || header.num_channels > 4)
		{
			throw Diagnostics::Error("Unable to read EXR source metadata.");
		}
		const std::int64_t admittedWidth = static_cast<std::int64_t>(header.data_window.max_x) - header.data_window.min_x + 1;
		const std::int64_t admittedHeight = static_cast<std::int64_t>(header.data_window.max_y) - header.data_window.min_y + 1;
		return MakeFlatTextureMetadata(admittedWidth, admittedHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);
	}
}

TextureSource LoadExrTextureSource(std::span<const std::uint8_t> bytes, const TextureCookRequest& request, TextureCookMemoryLimiter& processingMemory)
{
	const auto metadata = ExrSourceLoading::ReadMetadata(bytes);
	TextureSource source;
	source.ProcessingMemory = processingMemory.Acquire(MeasureTextureProcessingMemory(metadata, bytes.size(), request));

	int width = 0;
	int height = 0;
	float* decoded = nullptr;
	const int loaded = LoadEXRFromMemory(&decoded, &width, &height, bytes.data(), bytes.size(), nullptr);
	std::unique_ptr<float, decltype(&std::free)> pixels(decoded, &std::free);
	if (loaded != TINYEXR_SUCCESS || !pixels || width != metadata.width || height != metadata.height)
	{
		throw Diagnostics::Error("EXR source decode failed or disagreed with admitted metadata.");
	}

	CheckTextureOperation(source.Images.Initialize(metadata), "allocate imported EXR");
	std::memcpy(source.Images.GetPixels(), pixels.get(), source.Images.GetPixelsSize());
	return source;
}
