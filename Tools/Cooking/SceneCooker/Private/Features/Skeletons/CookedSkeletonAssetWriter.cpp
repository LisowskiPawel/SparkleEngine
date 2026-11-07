#include "PCH.h"

#include "CookedSkeletonAssetWriter.h"

#include "CookedSceneBuild.h"
#include "BinaryAssetWriter.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Files/FileSetPublication.h"
#include "Core/Public/Paths/ProductFilePaths.h"

namespace CookedSkeletonAssetWriter
{
	static void StageSkeletonAsset(const CookedSkeletonAssetBuild& skeletonAsset, std::vector<Files::FilePublication>& outPublication)
	{
		const std::filesystem::path outputPath = Paths::CookedSkeletonAsset(skeletonAsset.assetId);
		const std::filesystem::path stagedOutputPath = Files::BuildTemporaryPath(outputPath, ".cook-generation");

		Files::CleanupTemporaryFile(stagedOutputPath);
		outPublication.push_back({stagedOutputPath, outputPath});

		const Assets::CookedSkeletonAssetHeader header{
		    .fileHeader = {Assets::kCookedSkeletonAssetMagic},
		    .jointCount = static_cast<std::uint32_t>(skeletonAsset.joints.size()),
		    .jointStride = sizeof(Assets::CookedSkeletonJointRecord),
		    .flags = 0};

		SparkleTools::WriteBinaryAssetFile(stagedOutputPath, header, skeletonAsset.joints);
	}

	void StageSkeletonAssets(const std::vector<CookedSkeletonAssetBuild>& skeletonAssets, std::vector<Files::FilePublication>& outPublication)
	{
		for (const CookedSkeletonAssetBuild& skeletonAsset : skeletonAssets)
		{
			StageSkeletonAsset(skeletonAsset, outPublication);
		}
	}
}
