#pragma once

#include "Api/AssetCookerTypes.h"

#include <filesystem>
#include <string>
#include <vector>

class AssetCookerDiagnostics final
{
public:
	void AddError(std::string message, const std::filesystem::path& sourcePath = {});

	void Append(std::vector<AssetCookerDiagnosticRecord> records);
	std::vector<AssetCookerDiagnosticRecord> ReleaseRecords();

private:
	std::vector<AssetCookerDiagnosticRecord> m_records;
};
