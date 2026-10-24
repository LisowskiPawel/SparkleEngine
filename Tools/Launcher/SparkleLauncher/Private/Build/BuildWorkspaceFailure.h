#pragma once

#include <string>

namespace Process
{
	struct ChildProcessResult;
}

namespace SparkleLauncher
{
	struct BuildWorkspaceProcessStep;

	std::string MakeBuildWorkspaceFailureSummary(const BuildWorkspaceProcessStep& step, const Process::ChildProcessResult& result);
	std::string BuildWorkspaceRecoveryAction(const BuildWorkspaceProcessStep& step);
}
