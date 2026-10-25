#pragma once

#include <iosfwd>

namespace SparkleLauncher
{
	struct LauncherShellArguments;
	struct LauncherShellModel;

	bool ApplyLauncherShellDryRun(LauncherShellModel& model, const LauncherShellArguments& arguments);
	int RunLauncherShellOperation(LauncherShellModel& model, const LauncherShellArguments& arguments, std::ostream& output, std::ostream& error);
}
