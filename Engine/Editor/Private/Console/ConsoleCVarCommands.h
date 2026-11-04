#pragma once

#include "Core/Public/Console/ConsoleOutput.h"
#include "Core/Public/Console/CVarControl.h"

#include <string>
#include <string_view>
#include <vector>
#include <span>

class ConsoleVariableRegistry;

ConsoleCommandResult ExecuteConsoleListCVars(const CVarControlExecutor& executor, std::span<const std::string_view> arguments);
ConsoleCommandResult ExecuteConsoleGetCVar(const CVarControlExecutor& executor, std::span<const std::string_view> arguments);
ConsoleCommandResult ExecuteConsoleSetCVar(const CVarControlExecutor& executor, std::span<const std::string_view> arguments);
std::vector<std::string> CompleteConsoleCVarName(const ConsoleVariableRegistry& cvarRegistry, std::string_view prefix);
