#pragma once

#include "Level/LevelDesc.h"

#include <filesystem>
#include <iosfwd>

namespace LevelParsing
{
	void WriteLevelDescription(std::ostream& output, const LevelDesc& description);
}
