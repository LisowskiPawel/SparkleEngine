#include "LauncherGuiApp.h"
#include "../Private/Shell/LauncherShell.h"
#include "Core/Public/Threading/ThreadOwnership.h"
#include "Core/Public/Process/CommandLineUtils.h"
#include <vector>

#include <iostream>
#include <string_view>

namespace SparkleLauncherMain
{
	bool ShouldRunShell(int argc, char** argv) noexcept
	{
		for (int index = 1; index < argc; ++index)
		{
			const std::string_view argument(argv[index]);
			if (argument == "--level" || argument == "--product" || argument == "--graphics-api" || argument == "--gpu-crash-debugging" || argument == "--shader-export" || argument == "--dry-run"
			    || argument == "--run" || argument == "--help" || argument == "-h" || argument == "/?")
			{
				return true;
			}
		}
		return false;
	}
}

int main(int argc, char** argv)
{
	Threading::SetCurrentThreadRole("Sparkle.ToolMain");
#if defined(_WIN32)
	auto arguments = CommandLine::ReadProcessArguments();
	std::vector<char*> argumentPointers;
	for (auto& argument : arguments)
	{
		argumentPointers.push_back(argument.data());
	}
	argumentPointers.push_back(nullptr);
	argc = static_cast<int>(arguments.size());
	argv = argumentPointers.data();
#endif
#if !defined(_WIN32)
	std::cerr << "SparkleLauncher GUI is currently implemented for Windows.\n";
#endif
	if (SparkleLauncherMain::ShouldRunShell(argc, argv))
	{
		const SparkleLauncher::LauncherShell shell;
		return shell.Run(argc, argv, std::cout, std::cerr);
	}
	return SparkleLauncher::RunLauncherGui(argc, argv);
}
