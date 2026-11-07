#include "PCH.h"

#include "CookedAnimationAssetWriter.h"

#include "CookedSceneBuild.h"
#include "BinaryAssetWriter.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Files/FileSetPublication.h"
#include "Core/Public/Paths/ProductFilePaths.h"

#include <cstring>

namespace CookedAnimationAssetWriter
{
	static void BuildHeader(const CookedAnimationAssetBuild& animationAsset, Assets::CookedAnimationAssetHeader& header) noexcept
	{
		// The binary header includes alignment bytes that must be deterministic too.
		std::memset(&header, 0, sizeof(header));
		header.fileHeader.magic = Assets::kCookedAnimationAssetMagic;
		header.targetSkeletonAssetId = animationAsset.targetSkeletonAssetId;
		header.sourceAnimationIndex = animationAsset.sourceAnimationIndex;
		header.durationSeconds = animationAsset.durationSeconds;
		header.channelCount = static_cast<std::uint32_t>(animationAsset.channels.size());
		header.keyframeCount = static_cast<std::uint32_t>(animationAsset.keyframes.size());
		header.channelStride = sizeof(Assets::CookedAnimationChannelRecord);
		header.keyframeStride = sizeof(Assets::CookedAnimationKeyframeRecord);
		std::memcpy(header.name, animationAsset.name.data(), animationAsset.name.size());
	}

	static void StageAnimationAsset(const CookedAnimationAssetBuild& animationAsset, std::vector<Files::FilePublication>& outPublication)
	{
		const std::filesystem::path outputPath = Paths::CookedAnimationAsset(animationAsset.assetId);
		const std::filesystem::path stagedOutputPath = Files::BuildTemporaryPath(outputPath, ".cook-generation");

		Files::CleanupTemporaryFile(stagedOutputPath);
		outPublication.push_back({stagedOutputPath, outputPath});

		Assets::CookedAnimationAssetHeader header;
		BuildHeader(animationAsset, header);
		SparkleTools::WriteBinaryAssetFile(stagedOutputPath, header, animationAsset.channels, animationAsset.keyframes);
	}

	void StageAnimationAssets(const std::vector<CookedAnimationAssetBuild>& animationAssets, std::vector<Files::FilePublication>& outPublication)
	{
		for (const CookedAnimationAssetBuild& animationAsset : animationAssets)
		{
			StageAnimationAsset(animationAsset, outPublication);
		}
	}
}
