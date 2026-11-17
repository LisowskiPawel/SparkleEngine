#include "PCH.h"

#include "Cooking/TextureAssetCooker.h"
#include "Cooking/CookedTextureAssetWriter.h"
#include "Cooking/TextureCookMemoryLimiter.h"
#include "Processing/TextureProcessor.h"

void CookTextureAsset(const TextureCookRequest& request, TextureCookMemoryLimiter& memoryLimiter, TextureCookMemoryLimiter& sourceMemory, std::stop_token cancellation)
{
	ValidateTextureCookRequest(request);

	auto sourceLease = sourceMemory.Acquire(2 * MaximumTextureSourceBytes, cancellation);
	TextureSource source = LoadTextureSource(request, memoryLimiter, cancellation);
	auto cookedTexture = ProcessTexture(request, std::move(source.Images), cancellation);

	WriteCookedTextureAsset(cookedTexture, request.policy.mipPolicy, request.outputPath, cancellation);
}
