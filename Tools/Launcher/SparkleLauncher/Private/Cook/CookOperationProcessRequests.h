#pragma once

#include "SparkleLauncher/CookOperations.h"

#include <vector>

namespace SparkleLauncher
{
	bool IncludesCookScope(const CookOperationPlan& plan, CookWorkspaceScope scope);
	std::vector<CookOperationStep> BuildCookProcessStepsForPlan(const CookOperationPlan& plan);
}
