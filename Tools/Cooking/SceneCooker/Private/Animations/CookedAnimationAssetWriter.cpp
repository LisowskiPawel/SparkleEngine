#include "PCH.h"

#include "Animations/CookedAnimationAssetWriter.h"

#include "CookedSceneBuild.h"
#include "Core/Public/Files/BinaryFileWriter.h"

#include <cstring>

void CookedAnimationAssetWriter::Write(const CookedAnimationAssetBuild& asset, const std::filesystem::path& outputPath)
{
	Assets::CookedAnimationAssetHeader header;
	// The binary header includes alignment bytes that must be deterministic too.
	std::memset(&header, 0, sizeof(header));
	header.fileHeader.magic = Assets::kCookedAnimationAssetMagic;
	header.targetSkeletonAssetId = asset.targetSkeletonAssetId;
	header.sourceAnimationIndex = asset.sourceAnimationIndex;
	header.durationSeconds = asset.durationSeconds;
	header.channelCount = static_cast<std::uint32_t>(asset.channels.size());
	header.keyframeCount = static_cast<std::uint32_t>(asset.keyframes.size());
	header.channelStride = sizeof(Assets::CookedAnimationChannelRecord);
	header.keyframeStride = sizeof(Assets::CookedAnimationKeyframeRecord);
	std::memcpy(header.name, asset.name.data(), asset.name.size());
	Files::WriteBinaryFile(outputPath, header, asset.channels, asset.keyframes);
}
