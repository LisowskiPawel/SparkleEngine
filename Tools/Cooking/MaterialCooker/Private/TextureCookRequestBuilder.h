#pragma once

#include "TextureCookRequestList.h"

#include <filesystem>

TextureCookRequest BuildTextureCookRequest(const std::filesystem::path& sourceTexturePath, TextureGroup textureGroup, TextureChannelMask channelMask = TextureChannelMask::Rgba);
