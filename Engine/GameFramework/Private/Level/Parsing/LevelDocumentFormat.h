#pragma once

#include "Level/LevelDesc.h"

#include <filesystem>
#include <iosfwd>

namespace LevelParsing
{
	LevelDesc ReadLevelDescription(std::istream& input, const std::filesystem::path& sourcePath);
	void WriteLevelDescription(std::ostream& output, const LevelDesc& description);
}
