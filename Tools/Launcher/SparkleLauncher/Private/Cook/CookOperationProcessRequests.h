#pragma once

#include "SparkleLauncher/CookOperations.h"

#include <filesystem>
#include <string>
#include <vector>

namespace SparkleLauncher
{
	struct CookOperationProcessStep
	{
		std::string Id;
		std::string DisplayName;
		Process::ChildProcessRequest Request;
		std::filesystem::path DestructivePath;
		bool bHasProcessRequest = true;
		bool bDeletesCookedOutputs = false;
	};

	bool IncludesCookScope(const CookOperationPlan& plan, CookWorkspaceScope scope);
	std::vector<CookOperationProcessStep> BuildCookProcessStepsForPlan(const CookOperationPlan& plan);
}
