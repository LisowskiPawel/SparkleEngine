#pragma once

#include "TextureCookRequestList.h"
#include "SourceLoading/TextureSource.h"

#include <stop_token>

DirectX::ScratchImage ProcessTexture(const TextureCookRequest& request, DirectX::ScratchImage source, std::stop_token cancellation);
