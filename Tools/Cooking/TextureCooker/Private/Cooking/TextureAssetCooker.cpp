#include "PCH.h"

#include "Cooking/TextureAssetCooker.h"
#include "Cooking/CookedTextureAssetWriter.h"
#include "Cooking/TextureCookMemoryLimiter.h"
#include "Pipeline/TexturePipeline.h"
#include "SourceLoading/TextureSourceLoader.h"

void CookTextureAsset(const TextureCookRequest& request, TextureCookMemoryLimiter& memoryLimiter)
{
	ValidateTextureCookRequest(request);

	TextureLoadResult loadResult = LoadTextureSource(request.sourcePath);
	auto pixelDataMemoryLease = memoryLimiter.Acquire(loadResult);
	TextureLoadResult cookedTexture = TexturePipeline::Process(request, std::move(loadResult));

	WriteCookedTextureAsset(cookedTexture, request.outputPath);
}
