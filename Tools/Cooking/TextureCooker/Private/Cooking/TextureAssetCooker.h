#pragma once

#include "TextureCookRequestList.h"
#include <stop_token>

class TextureCookMemoryLimiter;

void CookTextureAsset(const TextureCookRequest& request, TextureCookMemoryLimiter& memoryLimiter, TextureCookMemoryLimiter& sourceMemory, std::stop_token cancellation = {});
