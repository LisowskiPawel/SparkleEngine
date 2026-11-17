#include "ImportedSceneCooker.h"

#include "CookedMeshAssetBuilder.h"
#include "MaterialCooker.h"
#include "SceneCooker.h"
#include "Core/Public/Diagnostics/Error.h"
#include "SourceSceneImporter.h"

#include <string>

CookedSceneBuild ImportedSceneCooker::Build(const AssetCookerSceneEntry& sceneEntry, AssetCookerDiagnostics& diagnostics)
{
	SourceImportOutput importOutput;
	try
	{
		importOutput = ImportSourceScene(sceneEntry.sourcePath);
	}
	catch (const Diagnostics::Error& error)
	{
		diagnostics.AddError(AssetCookerCategory::SceneAssets, error.what(), sceneEntry.sourcePath);
		throw;
	}
	return BuildCookedScene(sceneEntry, importOutput, diagnostics);
}

CookedSceneBuild ImportedSceneCooker::BuildCookedScene(const AssetCookerSceneEntry& sceneEntry, const SourceImportOutput& importOutput, AssetCookerDiagnostics& diagnostics)
{
	CookedSceneBuild build;
	try
	{
		build.identity = SceneCooker::ResolveSceneIdentity(sceneEntry.sourcePath);
	}
	catch (const Diagnostics::Error& error)
	{
		diagnostics.AddError(AssetCookerCategory::SceneAssets, error.what(), sceneEntry.sourcePath);
		throw;
	}

	try
	{
		build.outputs.meshAssets = CookedMeshAssetBuilder::BuildMeshAssets(importOutput, build.identity.assetId);
	}
	catch (const Diagnostics::Error& error)
	{
		diagnostics.AddError(AssetCookerCategory::Meshes, error.what(), sceneEntry.sourcePath);
		throw;
	}

	try
	{
		build.outputs.materialAssets = MaterialCooker::BuildMaterialAssets(importOutput, build.identity.assetId);
	}
	catch (const Diagnostics::Error& error)
	{
		diagnostics.AddError(AssetCookerCategory::Materials, error.what(), sceneEntry.sourcePath);
		throw;
	}

	try
	{
		SceneCooker::BuildManifest(importOutput, build);
	}
	catch (const Diagnostics::Error& error)
	{
		diagnostics.AddError(AssetCookerCategory::SceneAssets, error.what(), sceneEntry.sourcePath);
		throw;
	}
	return build;
}
