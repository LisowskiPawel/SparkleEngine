#pragma once

#include "Cooking/TextureCookMemoryLimiter.h"
#include <DirectXTex.h>

#include <cstddef>
#include <cstdint>
#include <stop_token>
#include <span>

inline constexpr std::size_t MaximumTextureSourceBytes = 64ull * 1024ull * 1024ull;

struct TextureSource final
{
	TextureCookMemoryLimiter::Lease ProcessingMemory;
	DirectX::ScratchImage Images;
};

struct TextureCookRequest;

DirectX::TexMetadata MakeFlatTextureMetadata(int width, int height, DXGI_FORMAT format);
std::size_t MeasureTextureProcessingMemory(const DirectX::TexMetadata& metadata, std::size_t fileBytes, const TextureCookRequest& request);

TextureSource LoadTextureSource(const TextureCookRequest& request, TextureCookMemoryLimiter& processingMemory, std::stop_token cancellation);
TextureSource LoadKtxTextureSource(std::span<const std::uint8_t> bytes, const TextureCookRequest& request, TextureCookMemoryLimiter& processingMemory, std::stop_token cancellation);
TextureSource LoadExrTextureSource(std::span<const std::uint8_t> bytes, const TextureCookRequest& request, TextureCookMemoryLimiter& processingMemory, std::stop_token cancellation);
TextureSource LoadRasterTextureSource(std::span<const std::uint8_t> bytes, const TextureCookRequest& request, TextureCookMemoryLimiter& processingMemory, std::stop_token cancellation);
