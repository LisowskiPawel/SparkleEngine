#pragma once

#include "Core/Public/Console/ConsoleOutput.h"
#include "Core/Public/Console/CVarControl.h"

#include <string>
#include <span>

ConsoleCommandResult ExecuteConsoleListCVars(const CVarControlExecutor& executor, std::span<const std::string> arguments);
ConsoleCommandResult ExecuteConsoleGetCVar(const CVarControlExecutor& executor, std::span<const std::string> arguments);
ConsoleCommandResult ExecuteConsoleSetCVar(const CVarControlExecutor& executor, std::span<const std::string> arguments);
