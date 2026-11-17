#include "PCH.h"
#include "Level/Parsing/LevelParser.h"

#include "Level/Parsing/LevelDocumentFormat.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Level/LevelAsset.h"

#include <fstream>
#include <format>
#include <sstream>
#include <utility>

std::unique_ptr<LevelAsset> LevelParser::LoadFromFile(const std::filesystem::path& filePath)
{
	std::ifstream input(filePath);
	if (!input.is_open())
	{
		throw Diagnostics::Error(std::format("Failed to open level file '{}'.", filePath.string()));
	}

	return std::make_unique<LevelAsset>(LevelParsing::ReadLevelDescription(input, filePath), filePath);
}

void LevelParser::SaveToFile(const LevelDesc& description, const std::filesystem::path& sourcePath)
{
	if (sourcePath.empty())
	{
		throw Diagnostics::Error("Level has no source path.");
	}

	std::ostringstream output;
	LevelParsing::WriteLevelDescription(output, description);
	if (!output.good())
	{
		throw Diagnostics::Error(std::format("Failed to serialize level '{}'.", description.name));
	}

	std::string errorMessage;
	if (!Files::TryWriteAllTextAtomic(sourcePath, std::move(output).str(), errorMessage))
	{
		throw Diagnostics::Error(errorMessage);
	}
}
