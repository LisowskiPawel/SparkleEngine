#include "PCH.h"

#include "MaterialCooker.h"
#include "CookedMaterialAssetBuild.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/BinaryStreamWriter.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Files/FileSetPublication.h"
#include "Core/Public/Paths/ProductFilePaths.h"

#include <fstream>
#include <limits>

static void WriteMaterialName(std::ofstream& output, const CookedMaterialAssetBuild& materialAsset)
{
	if (materialAsset.name.empty())
	{
		return;
	}

	output.write(materialAsset.name.data(), static_cast<std::streamsize>(materialAsset.name.size()));
	if (!output.good())
	{
		throw Diagnostics::Error("Failed to write cooked material asset name payload.");
	}
}

static std::vector<Assets::CookedTextureReferenceRecord> BuildTextureReferenceRecords(const CookedMaterialAssetBuild& materialAsset)
{
	std::vector<Assets::CookedTextureReferenceRecord> records;
	records.reserve(materialAsset.textureReferences.size());
	for (const Assets::CookedTextureReference& reference : materialAsset.textureReferences)
	{
		if (reference.texturePath.size() > (std::numeric_limits<std::uint32_t>::max)())
		{
			throw Diagnostics::Error("Cooked material texture reference path is too large to serialize.");
		}

		records.push_back({.texturePathByteCount = static_cast<std::uint32_t>(reference.texturePath.size()), .textureGroup = reference.textureGroup, .mapping = reference.mapping});
	}

	return records;
}

static void WriteTextureReferencePaths(std::ofstream& output, const CookedMaterialAssetBuild& materialAsset)
{
	std::string errorMessage;
	for (const Assets::CookedTextureReference& reference : materialAsset.textureReferences)
	{
		if (!Files::BinaryStreamWriter::WriteBytes(output, reference.texturePath.data(), reference.texturePath.size(), errorMessage))
		{
			throw Diagnostics::Error(errorMessage);
		}
	}
}

static void WriteMaterialAsset(const CookedMaterialAssetBuild& materialAsset, const std::filesystem::path& stagedOutputPath)
{
	std::string errorMessage;
	std::ofstream output;
	if (!Files::TryOpenBinaryOutput(stagedOutputPath, output, errorMessage))
	{
		throw Diagnostics::Error(errorMessage);
	}
	if (!Files::BinaryStreamWriter::WriteValue(output, materialAsset.header, errorMessage))
	{
		throw Diagnostics::Error(errorMessage);
	}

	WriteMaterialName(output, materialAsset);
	const std::vector<Assets::CookedTextureReferenceRecord> records = BuildTextureReferenceRecords(materialAsset);
	if (!Files::BinaryStreamWriter::WriteArray(output, records, errorMessage))
	{
		throw Diagnostics::Error(errorMessage);
	}
	WriteTextureReferencePaths(output, materialAsset);
	if (!Files::TryCloseOutput(output, stagedOutputPath, errorMessage))
	{
		throw Diagnostics::Error(errorMessage);
	}
}

static void StageMaterialAsset(const CookedMaterialAssetBuild& materialAsset, std::vector<Files::FilePublication>& outPublication)
{
	const std::filesystem::path outputPath = Paths::CookedMaterialAsset(materialAsset.assetId);
	const std::filesystem::path stagedOutputPath = Files::BuildTemporaryPath(outputPath, ".cook-generation");

	Files::CleanupTemporaryFile(stagedOutputPath);
	outPublication.push_back({stagedOutputPath, outputPath});

	WriteMaterialAsset(materialAsset, stagedOutputPath);
}

void MaterialCooker::StageMaterialAssets(const std::vector<CookedMaterialAssetBuild>& materialAssets, std::vector<Files::FilePublication>& outPublication)
{
	for (const CookedMaterialAssetBuild& materialAsset : materialAssets)
	{
		StageMaterialAsset(materialAsset, outPublication);
	}
}
