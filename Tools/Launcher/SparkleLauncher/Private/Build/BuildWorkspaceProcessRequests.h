#pragma once

#include "SparkleLauncher/BuildWorkspaceOperations.h"

#include <vector>

namespace SparkleLauncher
{
	std::vector<BuildWorkspaceOperationStep> BuildProcessStepsForPlan(const BuildWorkspaceOperationPlan& plan);
}
