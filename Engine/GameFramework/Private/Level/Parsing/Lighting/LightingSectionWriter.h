#pragma once

#include "GameFramework/Public/Level/LevelDesc.h"

#include <iosfwd>

namespace LevelParsing
{
	void WriteLightingSectionValues(std::ostream& output, const LevelDesc& levelDesc);
}
