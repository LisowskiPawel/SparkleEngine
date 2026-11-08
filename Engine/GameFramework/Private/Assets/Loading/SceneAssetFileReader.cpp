#include "PCH.h"

#include "Assets/Loading/SceneAssetFileReader.h"

#include "Assets/Cooked/LoadedSceneManifest.h"
#include "Assets/Loaders/CookedAssetFileSet.h"
#include "Assets/Loaders/SceneManifestLoader.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Paths/ProductFilePaths.h"

#include <format>

namespace Assets
{
	static void ReadMeshFiles(const LoadedSceneManifest& manifest, CookedAssetFileSet& files)
	{
		for (const auto& reference : manifest.meshAssetReferences)
		{
			files.Read(Paths::CookedMeshAsset(reference.meshAssetId));
		}
	}

	static void ReadMaterialFiles(const LoadedSceneManifest& manifest, CookedAssetFileSet& files)
	{
		for (const auto& reference : manifest.materialAssetReferences)
		{
			files.Read(Paths::CookedMaterialAsset(reference));
		}
	}

	static void ReadSkeletonFiles(const LoadedSceneManifest& manifest, CookedAssetFileSet& files)
	{
		for (const auto& reference : manifest.skeletonRefs)
		{
			files.Read(Paths::CookedSkeletonAsset(reference.skeletonAssetId));
		}
	}

	static void ReadAnimationFiles(const LoadedSceneManifest& manifest, CookedAssetFileSet& files)
	{
		for (const auto& reference : manifest.animationReferences)
		{
			files.Read(Paths::CookedAnimationAsset(reference.animationAssetId));
		}
	}

	void SceneAssetFileReader::Read(const SceneAssetId& sceneAssetId, const std::filesystem::path& manifestRelativePath, LoadedSceneManifest& manifest, CookedAssetFileSet& files)
	{
		const std::filesystem::path manifestPath = Paths::CookedSceneManifestRelative(manifestRelativePath);
		try
		{
			files.Read(manifestPath);
			manifest = SceneManifestLoader::Decode(manifestPath, files.Get(manifestPath), files.GetAllocationBudget());
			files.Release(manifestPath);

			ReadMeshFiles(manifest, files);
			ReadMaterialFiles(manifest, files);
			ReadSkeletonFiles(manifest, files);
			ReadAnimationFiles(manifest, files);
		}
		catch (const Diagnostics::Error& error)
		{
			throw Diagnostics::Error(std::format("Could not load scene asset '{}' from '{}': {}", sceneAssetId.value, manifestPath.generic_string(), error.what()));
		}
	}
}
