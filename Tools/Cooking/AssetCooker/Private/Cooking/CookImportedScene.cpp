#include "CookImportedScene.h"

#include "CookedMeshAssetBuilder.h"
#include "MaterialCooker.h"
#include "SceneCooker.h"
#include "Core/Public/Diagnostics/Error.h"
#include "SourceSceneImporter.h"

CookedSceneBuild CookImportedScene(const std::filesystem::path& sourcePath, AssetCookerDiagnostics& diagnostics)
{
	try
	{
		const SourceImportOutput importOutput = ImportSourceScene(sourcePath);
		CookedSceneBuild build;
		build.identity = SceneCooker::ResolveSceneIdentity(sourcePath);
		build.outputs.meshAssets = CookedMeshAssetBuilder::BuildMeshAssets(importOutput, build.identity.assetId);
		build.outputs.materialAssets = MaterialCooker::BuildMaterialAssets(importOutput, build.identity.assetId);
		SceneCooker::BuildManifest(importOutput, build);
		return build;
	}
	catch (const Diagnostics::Error& error)
	{
		diagnostics.AddError(error.what(), sourcePath);
		throw;
	}
}
