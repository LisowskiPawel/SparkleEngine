#pragma once

#include "RHI/Public/Resources/RhiTextureUpload.h"

#include <cstdint>
#include <filesystem>
#include <limits>
#include <vector>

struct LoadedTextureData final
{
	RhiTextureUploadDesc Upload;
	TextureFormatIntent FormatIntent = TextureFormatIntent::Unknown;
};

struct CookedTextureFilePayload final
{
	std::filesystem::path ResolvedPath;
	std::vector<std::uint8_t> Bytes;
};

class CookedTextureLoader final
{
public:
	static CookedTextureFilePayload Read(const std::filesystem::path& texturePath);
	static LoadedTextureData Decode(const CookedTextureFilePayload& payload, std::uint64_t maximumDecodedBytes = std::numeric_limits<std::uint64_t>::max());

	CookedTextureLoader() = delete;
	~CookedTextureLoader() = delete;
};
