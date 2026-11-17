#pragma once

#include <stop_token>

#include "TextureCookRequestList.h"

namespace DirectX
{
	class ScratchImage;
}

void WriteCookedTextureAsset(const DirectX::ScratchImage& texture, TextureMipPolicy policy, const std::filesystem::path& outputPath, std::stop_token cancellation);
