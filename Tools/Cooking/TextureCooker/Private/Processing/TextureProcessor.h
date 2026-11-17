#pragma once

#include "TextureCookRequestList.h"
#include <DirectXTex.h>

#include <stop_token>
#include <string_view>

void CheckTextureCancellation(std::stop_token cancellation);
void CheckTextureOperation(HRESULT result, std::string_view operation);

bool CanPreserveTextureBlocks(const TextureCookRequest& request, DXGI_FORMAT format);
DirectX::ScratchImage ProcessTexture(const TextureCookRequest& request, DirectX::ScratchImage source, std::stop_token cancellation);
