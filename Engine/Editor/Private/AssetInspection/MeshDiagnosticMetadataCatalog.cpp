#include "PCH.h"

#include "AssetInspection/MeshDiagnosticMetadataCatalog.h"

#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Json/JsonReader.h"
#include "Core/Public/Paths/ProductFilePaths.h"

#include <algorithm>
#include <filesystem>
#include <optional>
#include <string>

static std::optional<MeshDiagnosticMetadata> LoadCookedMeshMetadata(std::uint64_t meshAssetId)
{
	std::filesystem::path metadataPath = Paths::CookedMeshAsset(meshAssetId);
	metadataPath += ".meta.json";
	std::string metadataText;
	std::string readError;
	if (!Files::TryReadAllText(metadataPath, metadataText, readError))
	{
		return std::nullopt;
	}

	std::string schema;
	if (!Json::TryReadStringProperty(metadataText, "schema", schema) || schema != "cooked-mesh-metadata")
	{
		return std::nullopt;
	}

	MeshDiagnosticMetadata metadata;
	Json::TryReadStringProperty(metadataText, "displayName", metadata.DisplayName);
	Json::TryReadStringProperty(metadataText, "source", metadata.SourcePath);
	return metadata;
}

void MeshDiagnosticMetadataCatalog::Update(std::span<const MeshDiagnosticsRow> rows)
{
	for (auto& [assetId, entry] : m_entries)
	{
		entry.bPresent = false;
	}
	for (const MeshDiagnosticsRow& row : rows)
	{
		if (row.MeshAssetId == 0)
		{
			continue;
		}
		auto [entry, bInserted] = m_entries.try_emplace(row.MeshAssetId);
		entry->second.bPresent = true;
		if (bInserted)
		{
			entry->second.Metadata = LoadCookedMeshMetadata(row.MeshAssetId);
		}
	}
	std::erase_if(m_entries, [](const auto& entry) { return !entry.second.bPresent; });
}

void MeshDiagnosticMetadataCatalog::Clear() noexcept
{
	m_entries.clear();
}

const MeshDiagnosticMetadata* MeshDiagnosticMetadataCatalog::Find(std::uint64_t meshAssetId) const noexcept
{
	const auto entry = m_entries.find(meshAssetId);
	if (entry == m_entries.end() || !entry->second.Metadata)
	{
		return nullptr;
	}
	return &*entry->second.Metadata;
}
