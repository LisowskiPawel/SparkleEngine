#include "PCH.h"

#include "CookedSceneGenerationWriter.h"

#include "CookedSceneBuild.h"
#include "CookedMeshAssetWriter.h"
#include "Animations/CookedAnimationAssetWriter.h"
#include "Skeletons/CookedSkeletonAssetWriter.h"
#include "CookedSceneManifestWriter.h"
#include "Assets/SceneAssetRegistry.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Files/FileSetPublication.h"
#include "Core/Public/Paths/CookedAssetPaths.h"
#include "Core/Public/Paths/PathUtils.h"
#include "MaterialCooker.h"

#include <vector>

namespace CookedSceneStaging
{
	static std::filesystem::path StagePath(const std::filesystem::path& outputPath, std::vector<Files::FilePublication>& publication)
	{
		const auto stagedPath = Files::BuildTemporaryPath(outputPath, ".cook-generation");
		Files::CleanupTemporaryFile(stagedPath);
		publication.push_back({stagedPath, outputPath});
		return stagedPath;
	}

	static void StageRegistry(std::span<const CookedSceneBuild* const> builds, std::vector<Files::FilePublication>& publication)
	{
		Assets::SceneAssetRegistry registry;
		registry.Load();
		for (const CookedSceneBuild* build : builds)
		{
			const auto relativePath = Paths::TryMakeRelativeUnderRoot(build->identity.manifestPath, Filesystem::GetCookedSceneManifestRootPath());
			if (!relativePath)
			{
				throw Diagnostics::Error("Failed to derive a relative cooked scene manifest path for scene asset id '" + build->identity.assetId + "'.");
			}
			registry.Upsert(build->identity.assetId, *relativePath);
		}

		const auto stagedPath = StagePath(Filesystem::GetSceneAssetRegistryPath(), publication);
		registry.Save(stagedPath);
	}

	static void StageAssets(std::span<const CookedSceneBuild* const> builds, std::vector<Files::FilePublication>& publication)
	{
		for (const CookedSceneBuild* build : builds)
		{
			if (build == nullptr)
			{
				throw Diagnostics::Error("Scene generation contains a null build.");
			}

			CookedMeshAssetWriter::StageMeshAssets(build->outputs.meshAssets, publication);
			MaterialCooker::StageMaterialAssets(build->outputs.materialAssets, publication);
			for (const auto& asset : build->outputs.skeletonAssets)
			{
				const auto stagedPath = StagePath(Paths::CookedSkeletonAsset(asset.assetId), publication);
				CookedSkeletonAssetWriter::Write(asset, stagedPath);
			}
			for (const auto& asset : build->outputs.animationAssets)
			{
				const auto stagedPath = StagePath(Paths::CookedAnimationAsset(asset.assetId), publication);
				CookedAnimationAssetWriter::Write(asset, stagedPath);
			}
		}
	}
}

void CookedSceneGenerationWriter::Publish(std::span<const CookedSceneBuild* const> builds)
{
	const std::filesystem::path roots[] = {Filesystem::GetCookedAssetRootPath()};
	Files::FileSetPublisher publisher(roots);
	std::vector<Files::FilePublication> publication;
	try
	{
		CookedSceneStaging::StageAssets(builds, publication);
		for (const CookedSceneBuild* build : builds)
		{
			const auto stagedPath = CookedSceneStaging::StagePath(build->identity.manifestPath, publication);
			CookedSceneManifestWriter::Write(build->manifest, stagedPath);
		}
		CookedSceneStaging::StageRegistry(builds, publication);

		std::string errorMessage;
		if (!publisher.Publish(publication, errorMessage))
		{
			throw Diagnostics::Error(errorMessage);
		}
	}
	catch (...)
	{
		for (const Files::FilePublication& file : publication)
		{
			Files::CleanupTemporaryFile(file.StagedPath);
		}
		throw;
	}
}
