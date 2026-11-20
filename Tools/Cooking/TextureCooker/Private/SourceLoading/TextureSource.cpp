#include "PCH.h"

#include "SourceLoading/TextureSource.h"
#include "Processing/TextureProcessor.h"
#include "TextureCookRequestList.h"
#include "SourceAssetPaths.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Paths/PathUtils.h"

#include <vector>
#include <Windows.h>
#include <objbase.h>

using namespace DirectX;

namespace TextureSourceLoading
{
	class ComApartment final
	{
	public:
		ComApartment();
		~ComApartment();
		ComApartment(const ComApartment&) = delete;
		ComApartment& operator=(const ComApartment&) = delete;

	private:
		HRESULT m_result;
	};

	ComApartment::ComApartment() :
	    m_result(CoInitializeEx(nullptr, COINIT_MULTITHREADED))
	{
		if (FAILED(m_result) && m_result != RPC_E_CHANGED_MODE)
		{
			throw Diagnostics::Error("Failed to initialize COM for source texture loading.");
		}
	}

	ComApartment::~ComApartment()
	{
		if (SUCCEEDED(m_result))
		{
			CoUninitialize();
		}
	}

	std::vector<std::uint8_t> ReadSource(const std::filesystem::path& path)
	{
		const auto resolved = SparkleTools::ResolveSourceAssetPath(path, SparkleTools::SourceAssetType::Texture);
		if (!resolved)
		{
			throw Diagnostics::Error("Unable to resolve source texture.");
		}
		std::vector<std::uint8_t> bytes;
		std::string error;
		if (!Files::TryReadAllBytes(*resolved, bytes, error, MaximumTextureSourceBytes))
		{
			throw Diagnostics::Error(error);
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
		else if (extension == L".jpg" || extension == L".jpeg" || extension == L".bmp" || extension == L".gif")
		{
			InitializeTextureWicApartment();
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

void InitializeTextureWicApartment()
{
	thread_local const ComApartment comApartment;
}

TextureSource LoadTextureSource(const TextureCookRequest& request, TextureCookMemoryLimiter& processingMemory)
{
	const auto bytes = ReadSource(request.sourcePath);
	const auto extension = Paths::GetLowercaseExtension(request.sourcePath);

	if (extension == L".ktx" || extension == L".ktx2")
	{
		return LoadKtxTextureSource(bytes, request, processingMemory);
	}
	if (extension == L".exr")
	{
		return LoadExrTextureSource(bytes, request, processingMemory);
	}
	if (extension == L".png" || extension == L".psd" || extension == L".pic" || extension == L".pnm" || extension == L".ppm" || extension == L".pgm" || extension == L".tga")
	{
		return LoadRasterTextureSource(bytes, request, processingMemory);
	}

	const auto metadata = ReadNativeMetadata(bytes, extension);
	TextureSource source;
	const auto requiredMemory = MeasureTextureProcessingMemory(metadata, bytes.size(), request);
	source.ProcessingMemory = processingMemory.Acquire(requiredMemory);
	source.Images = DecodeNativeSource(bytes, extension, metadata);
	return source;
}
