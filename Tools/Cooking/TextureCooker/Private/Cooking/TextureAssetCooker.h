#pragma once

#include "TextureCookRequestList.h"

class TextureCookMemoryLimiter;

void CookTextureAsset(const TextureCookRequest& request, TextureCookMemoryLimiter& memoryLimiter);
