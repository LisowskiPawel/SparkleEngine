#pragma once

#include <filesystem>

struct TextureLoadResult;

void WriteCookedTextureAsset(const TextureLoadResult& cookedTexture, const std::filesystem::path& outputPath);
