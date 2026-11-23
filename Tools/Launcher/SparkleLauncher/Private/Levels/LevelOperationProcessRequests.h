#pragma once

#include "SparkleLauncher/LevelOperations.h"

#include <vector>

namespace SparkleLauncher
{
	std::vector<LevelOperationStep> BuildLevelOperationSteps(const LevelOperationPlan& plan);
}
