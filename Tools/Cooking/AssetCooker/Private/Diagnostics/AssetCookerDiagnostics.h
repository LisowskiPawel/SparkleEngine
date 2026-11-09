#pragma once

#include "Api/AssetCookerTypes.h"

#include <filesystem>
#include <string>
#include <vector>

class AssetCookerDiagnostics final
{
public:
	void AddError(AssetCookerCategory category, std::string message);
	void AddError(AssetCookerCategory category, std::string message, const std::filesystem::path& sourcePath);

	void Append(std::vector<AssetCookerDiagnosticRecord> records);
	std::vector<AssetCookerDiagnosticRecord> ReleaseRecords();

private:
	void Add(AssetCookerCategory category, std::string message, std::string sourcePath);

	std::vector<AssetCookerDiagnosticRecord> m_records;
};
