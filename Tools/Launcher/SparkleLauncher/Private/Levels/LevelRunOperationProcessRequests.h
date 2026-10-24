#pragma once

#include "SparkleLauncher/LevelRunOperations.h"

#include <string>
#include <vector>

namespace SparkleLauncher
{
	struct LevelRunOperationProcessStep
	{
		std::string Id;
		std::string DisplayName;
		Process::ChildProcessRequest Request;
	};

	std::vector<LevelRunOperationProcessStep> BuildLevelRunProcessStepsForPlan(const LevelRunOperationPlan& plan);
}
