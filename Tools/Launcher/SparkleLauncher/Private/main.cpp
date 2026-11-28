#include "Gui/App/LauncherGuiApp.h"
#include "Shell/LauncherShell.h"
#include "Startup/LauncherArguments.h"
#include "ToolMain.h"

#include <iostream>

namespace SparkleLauncher
{
	static int RunLauncher(int argc, char** argv)
	{
#if !defined(_WIN32)
		std::cerr << "SparkleLauncher GUI is currently implemented for Windows.\n";
#endif
		if (ShouldRunLauncherShell(argc, argv))
		{
			return RunLauncherShell(argc, argv, std::cout, std::cerr);
		}
		return RunLauncherGui(argc, argv);
	}
}

int main(int argc, char** argv)
{
	return SparkleTools::RunToolMain(argc, argv, SparkleLauncher::RunLauncher);
}
