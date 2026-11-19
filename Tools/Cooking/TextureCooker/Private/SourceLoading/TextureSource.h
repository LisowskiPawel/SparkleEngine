#pragma once

#include "Cooking/TextureCookMemoryLimiter.h"
#include <DirectXTex.h>

#include <cstddef>
#include <cstdint>
#include <span>

inline constexpr std::size_t MaximumTextureSourceBytes = 64ull * 1024ull * 1024ull;

struct TextureSource final
{
	TextureCookMemoryLimiter::Lease ProcessingMemory;
	DirectX::ScratchImage Images;
};

struct TextureCookRequest;

void InitializeTextureWicApartment();
DirectX::TexMetadata MakeFlatTextureMetadata(std::int64_t width, std::int64_t height, DXGI_FORMAT format);
std::size_t MeasureTextureProcessingMemory(const DirectX::TexMetadata& metadata, std::size_t fileBytes, const TextureCookRequest& request);

TextureSource LoadTextureSource(const TextureCookRequest& request, TextureCookMemoryLimiter& processingMemory);
TextureSource LoadKtxTextureSource(std::span<const std::uint8_t> bytes, const TextureCookRequest& request, TextureCookMemoryLimiter& processingMemory);
TextureSource LoadExrTextureSource(std::span<const std::uint8_t> bytes, const TextureCookRequest& request, TextureCookMemoryLimiter& processingMemory);
TextureSource LoadRasterTextureSource(std::span<const std::uint8_t> bytes, const TextureCookRequest& request, TextureCookMemoryLimiter& processingMemory);
