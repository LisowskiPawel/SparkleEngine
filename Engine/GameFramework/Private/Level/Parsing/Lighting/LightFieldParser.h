#pragma once

#include "GameFramework/Public/Scene/Lighting/SceneLightDesc.h"
#include "Level/Parsing/LevelParserCommon.h"

#include <vector>

namespace LevelParsing
{
	void ParseLightField(const ParsedLevelLine& parsedLine, std::vector<SceneLightDesc>& lights);
}
