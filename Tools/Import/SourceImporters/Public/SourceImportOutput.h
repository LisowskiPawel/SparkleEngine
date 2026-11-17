#pragma once

#include "Types/ImportedScene.h"

#include <filesystem>
#include <string>

struct SourceImportProvenance
{
	std::filesystem::path sourcePath;
	std::string importerId;
	float sourceMetersPerUnit = 0.0f;
};

struct SourceImportOutput
{
	ImportedScene scene;
	SourceImportProvenance provenance;

	const std::filesystem::path& GetSourcePath() const noexcept { return provenance.sourcePath; }
};
