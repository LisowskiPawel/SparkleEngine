#pragma once

#include "GameFramework/Public/Level/LevelDesc.h"
#include "Level/Parsing/LevelParserCommon.h"

#include <iosfwd>

namespace LevelParsing
{
	void ParseSkySectionField(const ParsedLevelLine& parsedLine, LevelDesc& levelDesc);
	void WriteSkySection(std::ostream& output, const LevelDesc& levelDesc);
}
