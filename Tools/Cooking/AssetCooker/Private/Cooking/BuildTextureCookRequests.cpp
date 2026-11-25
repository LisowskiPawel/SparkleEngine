#include "BuildTextureCookRequests.h"

#include "Core/Public/Diagnostics/Error.h"
#include "MaterialCooker.h"
#include "SourceSceneImporter.h"
#include "TextureCookRequestList.h"
#include "Cooking/DefaultTextureCookRequestBuilder.h"

#include <string>
#include <utility>

bool BuildTextureCookRequests(std::span<const AssetCookerSceneEntry> sceneEntries, AssetCookerDiagnostics& diagnostics, const std::filesystem::path& outputPath)
{
	TextureCookRequestSet requestSet;
	std::size_t failedSceneCount = 0;
	for (const AssetCookerSceneEntry& sceneEntry : sceneEntries)
	{
		try
		{
			const SourceImportOutput importOutput = ImportSourceScene(sceneEntry.sourcePath);
			MaterialCooker::AppendTextureCookRequests(importOutput, requestSet);
		}
		catch (const Diagnostics::Error& error)
		{
			diagnostics.AddError(error.what(), sceneEntry.sourcePath);
			++failedSceneCount;
		}
	}

	if (failedSceneCount != 0)
	{
		diagnostics.AddError("Texture request collection failed for " + std::to_string(failedSceneCount) + " scene(s).");
		return false;
	}

	try
	{
		AppendDefaultTextureCookRequests(requestSet);
	}
	catch (const Diagnostics::Error& error)
	{
		diagnostics.AddError(error.what());
		return false;
	}

	try
	{
		WriteTextureCookRequestList(outputPath, std::move(requestSet));
	}
	catch (const Diagnostics::Error& error)
	{
		diagnostics.AddError(error.what(), outputPath);
		return false;
	}

	return true;
}
