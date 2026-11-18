#pragma once

#include "TextureCookRequestList.h"

namespace DirectX
{
	class ScratchImage;
}

void StageCookedTextureAsset(const DirectX::ScratchImage& texture, TextureMipPolicy policy, const std::filesystem::path& stagedOutputPath);
