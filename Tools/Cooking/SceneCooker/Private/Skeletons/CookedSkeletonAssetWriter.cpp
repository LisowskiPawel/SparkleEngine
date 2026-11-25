#include "PCH.h"

#include "Skeletons/CookedSkeletonAssetWriter.h"

#include "CookedSceneBuild.h"
#include "Core/Public/Files/BinaryFileWriter.h"

void CookedSkeletonAssetWriter::Write(const CookedSkeletonAssetBuild& asset, const std::filesystem::path& outputPath)
{
	const Assets::CookedSkeletonAssetHeader header{
	    .fileHeader = {Assets::kCookedSkeletonAssetMagic},
	    .jointCount = static_cast<std::uint32_t>(asset.joints.size()),
	    .jointStride = sizeof(Assets::CookedSkeletonJointRecord),
	    .evaluationJointCount = static_cast<std::uint32_t>(asset.evaluationJoints.size()),
	    .evaluationJointStride = sizeof(Assets::CookedSkeletonEvaluationJointRecord),
	    .flags = 0};

	Files::WriteBinaryFile(outputPath, header, asset.joints, asset.evaluationJoints);
}
