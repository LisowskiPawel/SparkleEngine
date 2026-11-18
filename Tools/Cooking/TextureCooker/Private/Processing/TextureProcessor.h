#pragma once

#include "TextureCookRequestList.h"
#include <DirectXTex.h>

#include <string_view>

void CheckTextureOperation(HRESULT result, std::string_view operation);

bool CanPreserveTextureBlocks(const TextureCookRequest& request, DXGI_FORMAT format);
DirectX::ScratchImage ProcessTexture(const TextureCookRequest& request, DirectX::ScratchImage source);
