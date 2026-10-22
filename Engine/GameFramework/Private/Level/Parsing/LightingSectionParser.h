#pragma once

#include "GameFramework/Public/Level/LevelDesc.h"
#include "Level/Parsing/LevelParserCommon.h"

#include <iosfwd>

namespace LevelParsing
{
	void ParseLightingSectionField(const ParsedLevelLine& parsedLine, LevelDesc& levelDesc);
	void ValidateLightingSection(const LevelDesc& levelDesc);
	void WriteLightingSection(std::ostream& output, const LevelDesc& levelDesc);
}
