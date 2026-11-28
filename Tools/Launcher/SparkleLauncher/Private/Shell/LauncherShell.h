#pragma once

#include <iosfwd>

namespace SparkleLauncher
{
	int RunLauncherShell(int argc, char** argv, std::ostream& output, std::ostream& error);
}
