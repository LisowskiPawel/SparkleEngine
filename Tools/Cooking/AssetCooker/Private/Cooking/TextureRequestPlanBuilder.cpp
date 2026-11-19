#include "TextureRequestPlanBuilder.h"

#include "Core/Public/Diagnostics/Error.h"
#include "DefaultTextureCookRequestBuilder.h"
#include "MaterialCooker.h"
#include "SourceSceneImporter.h"
#include "TextureCookRequestList.h"

#include <string>
#include <utility>

void TextureRequestPlanBuilder::CollectSceneRequests(const AssetCookerSceneEntry& sceneEntry, TextureCookRequestSet& requestSet)
{
	const SourceImportOutput importOutput = ImportSourceScene(sceneEntry.sourcePath);
	MaterialCooker::AppendTextureCookRequests(importOutput, requestSet);
}

bool TextureRequestPlanBuilder::Build(const AssetCookerProjectCookPlan& plan, AssetCookerDiagnostics& diagnostics, const std::filesystem::path& outputPath)
{
	TextureCookRequestSet requestSet;
	std::size_t failedSceneCount = 0;
	for (std::size_t sceneIndex = 0; sceneIndex < plan.sceneEntries.size(); ++sceneIndex)
	{
		const AssetCookerSceneEntry& sceneEntry = plan.sceneEntries[sceneIndex];
		try
		{
			CollectSceneRequests(sceneEntry, requestSet);
		}
		catch (const Diagnostics::Error& error)
		{
			diagnostics.AddError(AssetCookerCategory::Textures, error.what(), sceneEntry.sourcePath);
			++failedSceneCount;
		}
	}

	if (failedSceneCount != 0)
	{
		diagnostics.AddError(AssetCookerCategory::Textures, "Texture request collection failed for " + std::to_string(failedSceneCount) + " scene(s).");
		return false;
	}

	try
	{
		DefaultTextureCookRequestBuilder::AppendTo(requestSet);
	}
	catch (const Diagnostics::Error& error)
	{
		diagnostics.AddError(AssetCookerCategory::Textures, error.what());
		return false;
	}

	try
	{
		WriteTextureCookRequestList(outputPath, std::move(requestSet));
	}
	catch (const Diagnostics::Error& error)
	{
		diagnostics.AddError(AssetCookerCategory::Textures, error.what(), outputPath);
		return false;
	}

	return true;
}
