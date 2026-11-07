#pragma once

#include "Core/Public/CoreAPI.h"

#include <cstdint>
#include <string>

enum class ConsoleCommandSeverity : std::uint8_t
{
	Info = 0,
	Warning,
	Error,
};

struct SPARKLE_CORE_API ConsoleCommandResult final
{
	bool bSucceeded = true;
	ConsoleCommandSeverity Severity = ConsoleCommandSeverity::Info;
	std::string Message;

	static ConsoleCommandResult Success(std::string message = {});
	static ConsoleCommandResult Warning(std::string message);
	static ConsoleCommandResult Error(std::string message);
};

struct SPARKLE_CORE_API ConsoleOutputRecord final
{
	ConsoleCommandSeverity Severity = ConsoleCommandSeverity::Info;
	std::string Text;
};
