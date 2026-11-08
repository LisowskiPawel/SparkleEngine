#include "PCH.h"

#include "CookedSceneGenerationWriter.h"

#include "CookedSceneBuild.h"
#include "CookedAnimationAssetWriter.h"
#include "CookedMeshAssetWriter.h"
#include "CookedSkeletonAssetWriter.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Files/FileSetPublication.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "MaterialCooker.h"
#include "CookedSceneManifestWriter.h"

#include <vector>

namespace CookedSceneStaging
{
	static void StageAssets(std::span<const CookedSceneBuild* const> builds, std::vector<Files::FilePublication>& outPublication);
	static void Cleanup(std::span<const Files::FilePublication> publication) noexcept;
}

void CookedSceneGenerationWriter::Publish(std::span<const CookedSceneBuild* const> builds)
{
	const std::filesystem::path roots[] = {Filesystem::GetCookedAssetRootPath()};
	Files::FileSetPublisher publisher(roots);
	std::vector<Files::FilePublication> publication;
	try
	{
		CookedSceneStaging::StageAssets(builds, publication);
		StageCookedSceneManifestsAndRegistry(builds, publication);
		std::string errorMessage;
		if (!publisher.Publish(publication, errorMessage))
		{
			throw Diagnostics::Error(errorMessage);
		}
	}
	catch (...)
	{
		CookedSceneStaging::Cleanup(publication);
		throw;
	}
}

void CookedSceneStaging::StageAssets(std::span<const CookedSceneBuild* const> builds, std::vector<Files::FilePublication>& outPublication)
{
	for (const CookedSceneBuild* build : builds)
	{
		if (build == nullptr)
		{
			throw Diagnostics::Error("Scene generation contains a null build.");
		}

		CookedMeshAssetWriter::StageMeshAssets(build->outputs.meshAssets, outPublication);
		MaterialCooker::StageMaterialAssets(build->outputs.materialAssets, outPublication);
		CookedSkeletonAssetWriter::StageSkeletonAssets(build->outputs.skeletonAssets, outPublication);
		CookedAnimationAssetWriter::StageAnimationAssets(build->outputs.animationAssets, outPublication);
	}
}

void CookedSceneStaging::Cleanup(std::span<const Files::FilePublication> publication) noexcept
{
	for (const Files::FilePublication& file : publication)
	{
		Files::CleanupTemporaryFile(file.StagedPath);
	}
}
