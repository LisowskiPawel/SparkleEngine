#include "../Private/App/TextureCookerApplication.h"

#include "Core/Public/Process/CommandLineUtils.h"
#include "Core/Public/Threading/ThreadOwnership.h"

#include <vector>

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
	return RunTextureCooker(argc, argv);
}
