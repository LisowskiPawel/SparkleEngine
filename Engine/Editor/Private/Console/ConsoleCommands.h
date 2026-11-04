#pragma once

#include "Core/Public/Console/CVarControl.h"
#include "Core/Public/Console/ConsoleOutput.h"

#include <functional>
#include <string>
#include <string_view>
#include <vector>

// The session owns these synchronous actions; UI composition binds their existing execution owners.
struct ConsoleCommandActions final
{
	CVarControlExecutor Variables;
	std::function<ConsoleCommandResult(bool)> CompileShaders;
	std::function<ConsoleCommandResult()> ReloadShaders;
};

ConsoleCommandResult ExecuteConsoleLine(const ConsoleCommandActions& actions, std::string_view input);
std::vector<std::string> CompleteConsoleLine(std::string_view input);
