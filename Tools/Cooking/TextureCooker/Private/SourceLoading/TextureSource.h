#pragma once

#include "Cooking/TextureCookMemoryLimiter.h"
#include <DirectXTex.h>

#include <cstddef>
#include <cstdint>
#include <stop_token>
#include <span>
#include <string_view>

inline constexpr std::size_t MaximumTextureSourceBytes = 64ull * 1024ull * 1024ull;

struct TextureSource final
{
	TextureCookMemoryLimiter::Lease ProcessingMemory;
	DirectX::ScratchImage Images;
};

struct TextureCookRequest;

void CheckTextureCancellation(std::stop_token cancellation);
void CheckTextureOperation(HRESULT result, std::string_view operation);
bool CanPreserveTextureBlocks(const TextureCookRequest& request, DXGI_FORMAT format);
std::size_t MeasureTextureProcessingMemory(const DirectX::TexMetadata& metadata, std::size_t fileBytes, const TextureCookRequest& request);
TextureSource LoadTextureSource(const TextureCookRequest& request, TextureCookMemoryLimiter& processingMemory, std::stop_token cancellation);
TextureSource LoadKtxTextureSource(std::span<const std::uint8_t> bytes, const TextureCookRequest& request, TextureCookMemoryLimiter& processingMemory, std::stop_token cancellation);
