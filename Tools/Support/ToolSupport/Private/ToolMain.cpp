#include "ToolMain.h"

#include "Core/Public/Process/CommandLineUtils.h"
#include "Core/Public/Threading/ThreadOwnership.h"

#include <vector>

namespace SparkleTools
{
	int RunToolMain(int argc, char** argv, ToolMain& runTool)
	{
		Threading::SetCurrentThreadRole("Sparkle.ToolMain");
#if defined(_WIN32)
		auto arguments = CommandLine::ReadProcessArguments();
		std::vector<char*> argumentPointers;
		argumentPointers.reserve(arguments.size() + 1);
		for (auto& argument : arguments)
		{
			argumentPointers.push_back(argument.data());
		}
		argumentPointers.push_back(nullptr);
		argc = static_cast<int>(arguments.size());
		argv = argumentPointers.data();
#endif
		return runTool(argc, argv);
	}
}
