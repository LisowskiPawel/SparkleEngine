#include "PCH.h"

#include "Core/Public/Console/ConsoleOutput.h"

#include <utility>

ConsoleCommandResult ConsoleCommandResult::Success(std::string message)
{
	return ConsoleCommandResult{.Succeeded = true, .Severity = ConsoleCommandSeverity::Info, .Message = std::move(message)};
}

ConsoleCommandResult ConsoleCommandResult::Warning(std::string message)
{
	return ConsoleCommandResult{.Succeeded = true, .Severity = ConsoleCommandSeverity::Warning, .Message = std::move(message)};
}

ConsoleCommandResult ConsoleCommandResult::Error(std::string message)
{
	return ConsoleCommandResult{.Succeeded = false, .Severity = ConsoleCommandSeverity::Error, .Message = std::move(message)};
}
