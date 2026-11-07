#include "PCH.h"

#include "Console/ConsoleCommands.h"
#include "Console/ConsoleCVarCommands.h"
#include "Console/ConsoleInputParser.h"
#include "Core/Public/Console/CVarRegistry.h"
#include "Core/Public/Strings/StringUtils.h"

#include <span>

struct ConsoleCommandEntry final
{
	std::string_view Name;
	std::string_view Help;
	std::string_view ArgumentSyntax;
	ConsoleCommandResult (*Execute)(const ConsoleCommandActions&, std::span<const std::string_view>);
	bool bCompletesCVarName = false;
};

static ConsoleCommandResult ExecuteConsoleHelp(const ConsoleCommandActions&, std::span<const std::string_view> arguments);
static ConsoleCommandResult ExecuteConsoleCompileShaders(const ConsoleCommandActions& actions, std::span<const std::string_view> arguments);
static ConsoleCommandResult ExecuteConsoleReloadShaders(const ConsoleCommandActions& actions, std::span<const std::string_view>);

static constexpr ConsoleCommandEntry kCommands[] = {
    {"Help", "Lists console commands or filters command help.", "[filter]", ExecuteConsoleHelp},
    {"ListCVars",
        "Lists registered console variables.",
        "[filter]",
        [](const ConsoleCommandActions& actions, std::span<const std::string_view> arguments) { return ExecuteConsoleListCVars(actions.Variables, arguments); },
        true},
    {"GetCVar",
        "Prints a console variable value.",
        "<name>",
        [](const ConsoleCommandActions& actions, std::span<const std::string_view> arguments) { return ExecuteConsoleGetCVar(actions.Variables, arguments); },
        true},
    {"SetCVar",
        "Sets a console variable value.",
        "<name> <value>",
        [](const ConsoleCommandActions& actions, std::span<const std::string_view> arguments) { return ExecuteConsoleSetCVar(actions.Variables, arguments); },
        true},
    {"CompileShaders", "Recompile through Launcher, then reload on success. Optional symbols exports private debug data.", "[symbols]", ExecuteConsoleCompileShaders},
    {"ReloadShaders", "Reloads the existing cooked shader files. Build/Cook runs in Launcher.", {}, ExecuteConsoleReloadShaders}};

static const ConsoleCommandEntry* FindCommand(std::string_view name)
{
	for (const ConsoleCommandEntry& command : kCommands)
	{
		if (Strings::EqualsIgnoreCase(command.Name, name))
		{
			return &command;
		}
	}
	return nullptr;
}

static std::vector<std::string_view> BuildArgumentViews(const std::vector<std::string>& arguments)
{
	std::vector<std::string_view> views;
	views.reserve(arguments.size());
	for (const std::string& argument : arguments)
	{
		views.push_back(argument);
	}
	return views;
}

ConsoleCommandResult ExecuteConsoleLine(const ConsoleCommandActions& actions, std::string_view input)
{
	const ConsoleParsedInput parsed = ConsoleInputParser::Parse(input);
	if (!parsed.bSucceeded)
	{
		return ConsoleCommandResult::Error(parsed.ErrorMessage);
	}
	if (parsed.IsEmpty())
	{
		return ConsoleCommandResult::Success();
	}
	const ConsoleCommandEntry* command = FindCommand(parsed.CommandName);
	if (command == nullptr)
	{
		return ConsoleCommandResult::Error("unknown command: " + parsed.CommandName);
	}
	return command->Execute(actions, BuildArgumentViews(parsed.Arguments));
}

std::vector<std::string> CompleteConsoleLine(std::string_view input)
{
	if (!Strings::ContainsAsciiWhitespace(input))
	{
		std::vector<std::string> completions;
		for (const ConsoleCommandEntry& command : kCommands)
		{
			if (Strings::StartsWithIgnoreCase(command.Name, input))
			{
				completions.emplace_back(command.Name);
			}
		}
		return completions;
	}
	const ConsoleParsedInput parsed = ConsoleInputParser::Parse(input);
	const ConsoleCommandEntry* command = parsed.bSucceeded ? FindCommand(parsed.CommandName) : nullptr;
	if (command == nullptr || !command->bCompletesCVarName)
	{
		return {};
	}
	return CompleteConsoleCVarName(ConsoleVariableRegistry::Get(), parsed.Arguments.empty() ? std::string_view{} : std::string_view(parsed.Arguments.back()));
}

static ConsoleCommandResult ExecuteConsoleHelp(const ConsoleCommandActions&, std::span<const std::string_view> arguments)
{
	if (arguments.size() > 1)
	{
		return ConsoleCommandResult::Error("usage: Help [filter]");
	}
	const std::string_view filter = arguments.empty() ? std::string_view{} : arguments.front();
	std::string output;
	for (const ConsoleCommandEntry& command : kCommands)
	{
		const bool bCommandByFilterName = !filter.empty() && !Strings::ContainsIgnoreCase(command.Name, filter);
		const bool bCommandExcludedByFilter = bCommandByFilterName && (!Strings::ContainsIgnoreCase(command.Help, filter));
		
		if (bCommandExcludedByFilter)
		{
			continue;
		}
		if (!output.empty())
		{
			output += '\n';
		}
		output += command.Name;
		if (!command.ArgumentSyntax.empty())
		{
			output += ' ';
			output += command.ArgumentSyntax;
		}
		output += " - ";
		output += command.Help;
	}
	return output.empty() ? ConsoleCommandResult::Warning("no commands matched") : ConsoleCommandResult::Success(std::move(output));
}

static ConsoleCommandResult ExecuteConsoleCompileShaders(const ConsoleCommandActions& actions, std::span<const std::string_view> arguments)
{
	if (arguments.size() > 1 || (!arguments.empty() && arguments.front() != "symbols"))
	{
		return ConsoleCommandResult::Error("Usage: CompileShaders [symbols]");
	}
	return actions.CompileShaders ? actions.CompileShaders(!arguments.empty()) : ConsoleCommandResult::Error("Shader compilation owner is unavailable.");
}

static ConsoleCommandResult ExecuteConsoleReloadShaders(const ConsoleCommandActions& actions, std::span<const std::string_view>)
{
	return actions.ReloadShaders ? actions.ReloadShaders() : ConsoleCommandResult::Error("Shader reload owner is unavailable.");
}
