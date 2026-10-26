#include "PCH.h"

#include "Console/ConsoleBuiltinCommandOperations.h"

#include "Core/Public/Console/CVar.h"
#include "Core/Public/Console/CVarRegistry.h"
#include "Core/Public/Strings/StringUtils.h"

#include <utility>

static ConsoleCommandResult FormatControlResult(CVarControlResult result);
static std::string FormatCommandHelp(std::string_view name, std::string_view arguments, std::string_view help);
static std::string FormatCVar(const CVarControlValue& variable);

ConsoleCommandResult ExecuteConsoleHelp(const ConsoleCommandRegistry& commandRegistry, ConsoleCommandScope scope, std::span<const std::string_view> arguments)
{
	if (arguments.size() > 1)
	{
		return ConsoleCommandResult::Error("usage: Help [filter]");
	}
	const std::string_view filter = arguments.empty() ? std::string_view{} : arguments.front();
	std::string output;
	for (const ConsoleCommandDescriptor& command : commandRegistry.GetCommands())
	{
		if (!ConsoleCommandRegistry::IsScopeAllowed(command.Scope, scope))
		{
			continue;
		}
		if (!filter.empty() && !Strings::ContainsIgnoreCase(command.Name, filter) && !Strings::ContainsIgnoreCase(command.Help, filter))
		{
			continue;
		}

		if (!output.empty())
		{
			output += '\n';
		}
		output += FormatCommandHelp(command.Name, command.ArgumentSyntax, command.Help);
	}

	if (output.empty())
	{
		return ConsoleCommandResult::Warning("no commands matched");
	}
	return ConsoleCommandResult::Success(output);
}

ConsoleCommandResult ExecuteConsoleListCVars(const CVarControlExecutor& executor, std::span<const std::string_view> arguments)
{
	if (arguments.size() > 1)
	{
		return ConsoleCommandResult::Error("usage: ListCVars [filter]");
	}
	if (!executor)
	{
		return ConsoleCommandResult::Error("CVar control owner is unavailable.");
	}
	return FormatControlResult(executor({.Operation = CVarControlOperation::List, .Filter = arguments.empty() ? std::string{} : std::string(arguments.front())}));
}

ConsoleCommandResult ExecuteConsoleGetCVar(const CVarControlExecutor& executor, std::span<const std::string_view> arguments)
{
	if (arguments.size() != 1)
	{
		return ConsoleCommandResult::Error("usage: GetCVar <name>");
	}

	if (!executor)
	{
		return ConsoleCommandResult::Error("CVar control owner is unavailable.");
	}
	return FormatControlResult(executor({.Operation = CVarControlOperation::Query, .Entries = {{std::string(arguments.front()), {}}}}));
}

ConsoleCommandResult ExecuteConsoleSetCVar(const CVarControlExecutor& executor, std::span<const std::string_view> arguments)
{
	if (arguments.size() < 2)
	{
		return ConsoleCommandResult::Error("usage: SetCVar <name> <value>");
	}

	if (!executor)
	{
		return ConsoleCommandResult::Error("CVar control owner is unavailable.");
	}
	return FormatControlResult(executor({.Operation = CVarControlOperation::Set, .Entries = {{std::string(arguments.front()), Strings::Join(arguments, " ", 1)}}}));
}

static ConsoleCommandResult FormatControlResult(CVarControlResult result)
{
	if (!result.Error.empty())
	{
		return ConsoleCommandResult::Error(std::move(result.Error));
	}
	std::string output;
	for (const CVarControlValue& value : result.Values)
	{
		if (!output.empty())
		{
			output += '\n';
		}
		output += FormatCVar(value);
	}
	return output.empty() ? ConsoleCommandResult::Warning("no CVars matched") : ConsoleCommandResult::Success(std::move(output));
}

std::vector<std::string> CompleteConsoleCVarName(const ConsoleVariableRegistry& cvarRegistry, std::string_view prefix)
{
	std::vector<std::string> completions;
	for (const ConsoleVariableBase* variable : cvarRegistry.GetVariables())
	{
		if (variable != nullptr && Strings::StartsWithIgnoreCase(variable->GetName(), prefix))
		{
			completions.emplace_back(variable->GetName());
		}
	}
	return completions;
}

static std::string FormatCommandHelp(std::string_view name, std::string_view arguments, std::string_view help)
{
	std::string output(name);
	if (!arguments.empty())
	{
		output += ' ';
		output += arguments;
	}
	if (!help.empty())
	{
		output += " - ";
		output += help;
	}
	return output;
}

static std::string FormatCVar(const CVarControlValue& variable)
{
	std::string output(variable.Name);
	output += " = ";
	output += variable.Value;
	output += " (";
	output += variable.Type;
	output += ')';
	if (!variable.Description.empty())
	{
		output += " - ";
		output += variable.Description;
	}
	return output;
}
