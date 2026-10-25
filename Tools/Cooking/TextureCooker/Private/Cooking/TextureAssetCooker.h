#pragma once

#include "TextureCookRequestList.h"

class TextureCookMemoryLimiter;

class TextureAssetCooker final
{
public:
	void Cook(const TextureCookRequest& request, TextureCookMemoryLimiter& memoryLimiter) const;
};
