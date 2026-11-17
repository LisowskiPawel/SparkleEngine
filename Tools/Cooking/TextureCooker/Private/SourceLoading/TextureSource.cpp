#include "PCH.h"

#include "SourceLoading/TextureSource.h"
#include "Processing/TextureProcessor.h"
#include "TextureCookRequestList.h"
#include "SourceAssetPaths.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Paths/PathUtils.h"

#include <fstream>
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

	constexpr auto WicFlags = static_cast<WIC_FLAGS>(WIC_FLAGS_FORCE_RGB | WIC_FLAGS_NO_16BPP | WIC_FLAGS_IGNORE_SRGB);

	TexMetadata ReadNativeMetadata(std::span<const std::uint8_t> bytes, std::wstring_view extension)
	{
		TexMetadata metadata{};
		HRESULT result = E_INVALIDARG;
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
			result = GetMetadataFromWICMemory(bytes.data(), bytes.size(), static_cast<WIC_FLAGS>(WicFlags | WIC_FLAGS_ALL_FRAMES), metadata);
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
		return metadata;
	}

	ScratchImage DecodeNativeSource(std::span<const std::uint8_t> bytes, std::wstring_view extension, const TexMetadata& admitted)
	{
		ScratchImage images;
		HRESULT result = E_INVALIDARG;
		if (extension == L".dds")
		{
			result = LoadFromDDSMemory(bytes.data(), bytes.size(), DDS_FLAGS_NONE, nullptr, images);
		}
		else if (extension == L".hdr" || extension == L".hdri")
		{
			result = LoadFromHDRMemory(bytes.data(), bytes.size(), nullptr, images);
		}
		else
		{
			result = LoadFromWICMemory(bytes.data(), bytes.size(), WicFlags, nullptr, images);
		}
		CheckTextureOperation(result, "decode source");

		const auto& loaded = images.GetMetadata();
		if (admitted.width != loaded.width || admitted.height != loaded.height || admitted.arraySize != loaded.arraySize || admitted.mipLevels != loaded.mipLevels || admitted.format != loaded.format)
		{
			throw Diagnostics::Error("Native source decode disagreed with admitted metadata.");
		}
		return images;
	}
}

using namespace TextureSourceLoading;

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
		return LoadExrTextureSource(bytes, request, processingMemory, cancellation);
	}
	if (extension == L".psd" || extension == L".pic" || extension == L".pnm" || extension == L".ppm" || extension == L".pgm" || extension == L".tga")
	{
		return LoadRasterTextureSource(bytes, request, processingMemory, cancellation);
	}

	const auto metadata = ReadNativeMetadata(bytes, extension);
	TextureSource source;
	source.ProcessingMemory = processingMemory.Acquire(MeasureTextureProcessingMemory(metadata, bytes.size(), request), cancellation);
	source.Images = DecodeNativeSource(bytes, extension, metadata);
	return source;
}
