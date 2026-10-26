#include "PCH.h"

#include "Core/Public/Console/ConsoleBuiltinCommands.h"

#include "Console/ConsoleBuiltinCommandOperations.h"
#include "Core/Public/Console/CVarRegistry.h"

#include <span>
#include <string_view>

void RegisterConsoleBuiltinCommands(ConsoleCommandRegistry& commandRegistry, CVarControlExecutor executor)
{
	auto& cvarRegistry = ConsoleVariableRegistry::Get();

	commandRegistry.Register(
	    ConsoleCommandDescriptor{
	        .Name = "Help",
	        .Help = "Lists console commands or filters command help.",
	        .ArgumentSyntax = "[filter]",
	        .Scope = ConsoleCommandScope::Game,
	        .Execute = [&commandRegistry](ConsoleCommandScope scope, std::span<const std::string_view> arguments) { return ExecuteConsoleHelp(commandRegistry, scope, arguments); },
	    });

	commandRegistry.Register(
	    ConsoleCommandDescriptor{
	        .Name = "ListCVars",
	        .Help = "Lists registered console variables.",
	        .ArgumentSyntax = "[filter]",
	        .Scope = ConsoleCommandScope::Game,
	        .Execute = [executor](ConsoleCommandScope, std::span<const std::string_view> arguments) { return ExecuteConsoleListCVars(executor, arguments); },
	        .Complete = [&cvarRegistry](ConsoleCommandScope, const ConsoleAutocompleteRequest& request) { return CompleteConsoleCVarName(cvarRegistry, request.CurrentToken); },
	    });

	commandRegistry.Register(
	    ConsoleCommandDescriptor{
	        .Name = "GetCVar",
	        .Help = "Prints a console variable value.",
	        .ArgumentSyntax = "<name>",
	        .Scope = ConsoleCommandScope::Game,
	        .Execute = [executor](ConsoleCommandScope, std::span<const std::string_view> arguments) { return ExecuteConsoleGetCVar(executor, arguments); },
	        .Complete = [&cvarRegistry](ConsoleCommandScope, const ConsoleAutocompleteRequest& request) { return CompleteConsoleCVarName(cvarRegistry, request.CurrentToken); },
	    });

	commandRegistry.Register(
	    ConsoleCommandDescriptor{
	        .Name = "SetCVar",
	        .Help = "Sets a console variable value.",
	        .ArgumentSyntax = "<name> <value>",
	        .Scope = ConsoleCommandScope::Game,
	        .Execute = [executor](ConsoleCommandScope, std::span<const std::string_view> arguments) { return ExecuteConsoleSetCVar(executor, arguments); },
	        .Complete = [&cvarRegistry](ConsoleCommandScope, const ConsoleAutocompleteRequest& request) { return CompleteConsoleCVarName(cvarRegistry, request.CurrentToken); },
	    });
}

