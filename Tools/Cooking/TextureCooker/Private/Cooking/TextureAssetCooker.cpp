#include "PCH.h"

#include "Cooking/TextureAssetCooker.h"
#include "Cooking/CookedTextureAssetWriter.h"
#include "Cooking/TextureCookMemoryLimiter.h"
#include "Processing/TextureProcessor.h"
#include "SourceLoading/TextureSource.h"

void CookTextureAsset(const TextureCookRequest& request, TextureCookMemoryLimiter& memoryLimiter, TextureCookMemoryLimiter& sourceMemory)
{
	ValidateTextureCookRequest(request);

	auto sourceLease = sourceMemory.Acquire(2 * MaximumTextureSourceBytes);
	TextureSource source = LoadTextureSource(request, memoryLimiter);
	auto cookedTexture = ProcessTexture(request, std::move(source.Images));

	StageCookedTextureAsset(cookedTexture, request.policy.mipPolicy, request.outputPath);
}
