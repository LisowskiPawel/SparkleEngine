#include "PCH.h"

#include "CookedMeshAssetWriter.h"

#include "CookedMeshAssetBuild.h"
#include "Core/Public/Files/BinaryFileWriter.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Files/FileSetPublication.h"

#include "Core/Public/Paths/CookedAssetPaths.h"

#include <cstdint>
#include <filesystem>

namespace CookedMeshAssetWriter
{
	static void StageMeshAsset(const CookedMeshAssetBuild& meshAsset, std::vector<Files::FilePublication>& outPublication);
	static void WriteMeshAsset(const CookedMeshAssetBuild& meshAsset, const std::filesystem::path& outputPath);
	static Assets::CookedMeshAssetHeader BuildHeader(const CookedMeshAssetBuild& meshAsset) noexcept;
}

void CookedMeshAssetWriter::StageMeshAsset(const CookedMeshAssetBuild& meshAsset, std::vector<Files::FilePublication>& outPublication)
{
	const std::filesystem::path outputPath = Paths::CookedMeshAsset(meshAsset.assetId);
	const std::filesystem::path stagedOutputPath = Files::BuildTemporaryPath(outputPath, ".cook-generation");

	Files::CleanupTemporaryFile(stagedOutputPath);

	outPublication.push_back({stagedOutputPath, outputPath});

	WriteMeshAsset(meshAsset, stagedOutputPath);
}

void CookedMeshAssetWriter::WriteMeshAsset(const CookedMeshAssetBuild& meshAsset, const std::filesystem::path& outputPath)
{
	const Assets::CookedMeshAssetHeader header = BuildHeader(meshAsset);

	Files::WriteBinaryFile(outputPath, header, meshAsset.vertices, meshAsset.indices, meshAsset.skinInfluences, meshAsset.morphTargets, meshAsset.morphTargetDeltas);
}

Assets::CookedMeshAssetHeader CookedMeshAssetWriter::BuildHeader(const CookedMeshAssetBuild& meshAsset) noexcept
{
	return Assets::CookedMeshAssetHeader{
	    .fileHeader = {Assets::kCookedMeshAssetMagic},
	    .vertexCount = static_cast<std::uint32_t>(meshAsset.vertices.size()),
	    .indexCount = static_cast<std::uint32_t>(meshAsset.indices.size()),
	    .skinInfluenceCount = static_cast<std::uint32_t>(meshAsset.skinInfluences.size()),
	    .vertexStride = sizeof(Assets::CookedMeshVertex),
	    .indexStride = sizeof(std::uint32_t),
	    .skinInfluenceStride = sizeof(Assets::CookedMeshSkinInfluence),
	    .morphTargetCount = static_cast<std::uint32_t>(meshAsset.morphTargets.size()),
	    .morphTargetDeltaCount = static_cast<std::uint32_t>(meshAsset.morphTargetDeltas.size()),
	    .morphTargetRecordStride = sizeof(Assets::CookedMeshMorphTargetRecord),
	    .morphTargetDeltaStride = sizeof(Assets::CookedMeshMorphTargetDelta),
	    .flags = (meshAsset.HasSkinInfluences() ? static_cast<std::uint32_t>(Assets::CookedMeshAssetFlag::HasSkinInfluences) : 0u)
	        | (meshAsset.HasMorphTargets() ? static_cast<std::uint32_t>(Assets::CookedMeshAssetFlag::HasMorphTargets) : 0u),
	    .assetKind = meshAsset.assetKind};
}

void CookedMeshAssetWriter::StageMeshAssets(const std::vector<CookedMeshAssetBuild>& meshAssets, std::vector<Files::FilePublication>& outPublication)
{
	for (const CookedMeshAssetBuild& meshAsset : meshAssets)
	{
		CookedMeshAssetWriter::StageMeshAsset(meshAsset, outPublication);
	}
}
