#pragma once

#include <string>

namespace Process
{
	struct ChildProcessResult;
}

namespace SparkleLauncher
{
	struct BuildWorkspaceOperationStep;

	std::string MakeBuildWorkspaceFailureSummary(const BuildWorkspaceOperationStep& step, const Process::ChildProcessResult& result);
	std::string BuildWorkspaceRecoveryAction(const BuildWorkspaceOperationStep& step);
}
