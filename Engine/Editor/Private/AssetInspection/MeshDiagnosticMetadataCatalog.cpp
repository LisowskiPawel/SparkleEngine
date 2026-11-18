#include "PCH.h"

#include "AssetInspection/MeshDiagnosticMetadataCatalog.h"

#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Json/JsonInputAdmission.h"
#include "Core/Public/Paths/CookedAssetPaths.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <array>
#include <filesystem>
#include <optional>
#include <set>
#include <string>
#include <string_view>

static std::optional<MeshDiagnosticMetadata> ParseCookedMeshMetadata(std::string_view metadataText)
{
	try
	{
		Json::InputAdmission admission(metadataText, metadataText.size());
		admission.BeginObject(0);
		std::set<std::string> repeated;
		const std::array<std::string_view, 3> fields{"schema", "displayName", "source"};

		auto document = nlohmann::json::parse(
		    metadataText.begin(),
		    metadataText.end(),
		    [&](int depth, nlohmann::json::parse_event_t event, nlohmann::json& value)
		    {
			    admission.CheckDepth(depth);
			    if (event != nlohmann::json::parse_event_t::key)
			    {
				    return true;
			    }

			    const auto& key = value.get_ref<const std::string&>();
			    const bool bConsumed = depth == 1 && std::find(fields.begin(), fields.end(), key) != fields.end();
			    if (bConsumed && !admission.TryReadKey(depth, key))
			    {
				    repeated.insert(key);
			    }
			    return bConsumed;
		    });

		for (const auto& key : repeated)
		{
			document.erase(key);
		}
		if (document.at("schema") != "cooked-mesh-metadata")
		{
			return std::nullopt;
		}

		MeshDiagnosticMetadata metadata;
		const auto display = document.find("displayName");
		if (display != document.end() && display->is_string())
		{
			metadata.DisplayName = display->get_ref<const std::string&>();
		}
		const auto source = document.find("source");
		if (source != document.end() && source->is_string())
		{
			metadata.SourcePath = source->get_ref<const std::string&>();
		}
		return metadata;
	}
	catch (const std::exception&)
	{
		return std::nullopt;
	}
}

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

	return ParseCookedMeshMetadata(metadataText);
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
