#include "PCH.h"

#include "CookedAnimationAssetWriter.h"

#include "CookedSceneBuild.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/BinaryStreamWriter.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Paths/DirectoryPaths.h"

#include <cstring>
#include <fstream>

class CookedAnimationAssetStager final
{
public:
	static void StageAnimationAsset(const CookedAnimationAssetBuild& animationAsset, std::vector<Files::FilePublication>& outPublication);
	static void BuildHeader(const CookedAnimationAssetBuild& animationAsset, Assets::CookedAnimationAssetHeader& header) noexcept;
};

void CookedAnimationAssetWriter::StageAnimationAssets(const std::vector<CookedAnimationAssetBuild>& animationAssets, std::vector<Files::FilePublication>& outPublication)
{
	for (const CookedAnimationAssetBuild& animationAsset : animationAssets)
	{
		CookedAnimationAssetStager::StageAnimationAsset(animationAsset, outPublication);
	}
}

void CookedAnimationAssetStager::StageAnimationAsset(const CookedAnimationAssetBuild& animationAsset, std::vector<Files::FilePublication>& outPublication)
{
	const std::filesystem::path outputPath = Paths::CookedAnimationAsset(animationAsset.assetId);
	const std::filesystem::path stagedOutputPath = Files::BuildTemporaryPath(outputPath, ".cook-generation");

	Files::CleanupTemporaryFile(stagedOutputPath);
	outPublication.push_back({stagedOutputPath, outputPath});

	Assets::CookedAnimationAssetHeader header;
	BuildHeader(animationAsset, header);
	std::string errorMessage;
	std::ofstream output;
	if (!Files::TryOpenBinaryOutput(stagedOutputPath, output, errorMessage) || !Files::BinaryStreamWriter::WriteValue(output, header, errorMessage)
	    || !Files::BinaryStreamWriter::WriteArray(output, animationAsset.channels, errorMessage) || !Files::BinaryStreamWriter::WriteArray(output, animationAsset.keyframes, errorMessage))
	{
		throw Diagnostics::Error(errorMessage);
	}

	if (!Files::TryCloseOutput(output, stagedOutputPath, errorMessage))
	{
		throw Diagnostics::Error(errorMessage);
	}
}

void CookedAnimationAssetStager::BuildHeader(const CookedAnimationAssetBuild& animationAsset, Assets::CookedAnimationAssetHeader& header) noexcept
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
