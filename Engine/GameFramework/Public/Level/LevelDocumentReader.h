#pragma once

#include "Level/LevelDesc.h"

#include <filesystem>
#include <iosfwd>

namespace LevelParsing
{
	SPARKLE_ENGINE_API LevelDesc ReadLevelDescription(std::istream& input, const std::filesystem::path& sourcePath);
}
