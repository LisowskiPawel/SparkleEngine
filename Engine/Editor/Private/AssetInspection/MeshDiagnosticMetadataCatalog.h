#pragma once

#include "Renderer/Public/Meshes/MeshDiagnostics.h"

#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <unordered_map>

struct MeshDiagnosticMetadata final
{
	std::string DisplayName;
	std::string SourcePath;
};

// One panel observation. Reopening or an explicit refresh admits new sidecar data.
class MeshDiagnosticMetadataCatalog final
{
public:
	void Update(std::span<const MeshDiagnosticsRow> rows);
	void Clear() noexcept;
	const MeshDiagnosticMetadata* Find(std::uint64_t meshAssetId) const noexcept;

private:
	struct Entry final
	{
		std::optional<MeshDiagnosticMetadata> Metadata;
		bool bPresent = false;
	};

	std::unordered_map<std::uint64_t, Entry> m_entries;
};
