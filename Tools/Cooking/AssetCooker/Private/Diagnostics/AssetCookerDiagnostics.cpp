#include "AssetCookerDiagnostics.h"

#include <iterator>
#include <utility>

void AssetCookerDiagnostics::AddError(std::string message, const std::filesystem::path& sourcePath)
{
	m_records.push_back({std::move(message), sourcePath.string()});
}

void AssetCookerDiagnostics::Append(std::vector<AssetCookerDiagnosticRecord> additionalRecords)
{
	m_records.insert(m_records.end(), std::make_move_iterator(additionalRecords.begin()), std::make_move_iterator(additionalRecords.end()));
}

std::vector<AssetCookerDiagnosticRecord> AssetCookerDiagnostics::ReleaseRecords()
{
	return std::move(m_records);
}
