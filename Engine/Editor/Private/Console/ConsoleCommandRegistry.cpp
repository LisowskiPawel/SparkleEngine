#include "PCH.h"

#include "Console/ConsoleCommandRegistry.h"

#include "Console/ConsoleInputParser.h"
#include "Core/Public/Strings/StringUtils.h"

#include <string>
#include <string_view>
#include <utility>
#include <vector>

bool ConsoleCommandRegistry::Register(ConsoleCommandDescriptor descriptor)
{
	if (descriptor.Name.empty())
	{
		return false;
	}
	if (!descriptor.Execute)
	{
		return false;
	}

	if (Find(descriptor.Name) != nullptr)
	{
		return false;
	}

	m_commands.push_back(std::move(descriptor));
	return true;
}

const ConsoleCommandDescriptor* ConsoleCommandRegistry::Find(std::string_view commandName) const
{
	for (const ConsoleCommandDescriptor& command : m_commands)
	{
		if (Strings::EqualsIgnoreCase(command.Name, commandName))
		{
			return &command;
		}
	}

	return nullptr;
}

ConsoleCommandResult ConsoleCommandRegistry::ExecuteLine(std::string_view input) const
{
	const ConsoleParsedInput parsedInput = ConsoleInputParser::Parse(input);
	if (!parsedInput.Succeeded)
	{
		return ConsoleCommandResult::Error(parsedInput.ErrorMessage);
	}
	if (parsedInput.IsEmpty())
	{
		return ConsoleCommandResult::Success();
	}

	const ConsoleCommandDescriptor* descriptor = Find(parsedInput.CommandName);
	if (descriptor == nullptr)
	{
		return ConsoleCommandResult::Error("unknown command: " + parsedInput.CommandName);
	}
	const std::vector<std::string_view> argumentViews = BuildArgumentViews(parsedInput.Arguments);
	return descriptor->Execute(argumentViews);
}

std::vector<std::string> ConsoleCommandRegistry::CompleteLine(std::string_view input) const
{
	const bool commandNameOnly = !Strings::ContainsAsciiWhitespace(input);
	if (commandNameOnly)
	{
		std::vector<std::string> completions;
		for (const ConsoleCommandDescriptor& command : m_commands)
		{
			if (Strings::StartsWithIgnoreCase(command.Name, input))
			{
				completions.push_back(command.Name);
			}
		}
		return completions;
	}

	const ConsoleParsedInput parsedInput = ConsoleInputParser::Parse(input);
	if (!parsedInput.Succeeded || parsedInput.CommandName.empty())
	{
		return {};
	}

	const ConsoleCommandDescriptor* descriptor = Find(parsedInput.CommandName);
	if (descriptor == nullptr || !descriptor->Complete)
	{
		return {};
	}

	const std::vector<std::string_view> argumentViews = BuildArgumentViews(parsedInput.Arguments);
	const std::string_view currentToken = argumentViews.empty() ? std::string_view{} : argumentViews.back();

	return descriptor->Complete(
	    ConsoleAutocompleteRequest{
	        .CommandName = descriptor->Name,
	        .Arguments = argumentViews,
	        .CurrentToken = currentToken,
	    });
}

std::vector<std::string_view> ConsoleCommandRegistry::BuildArgumentViews(const std::vector<std::string>& arguments)
{
	std::vector<std::string_view> argumentViews;
	argumentViews.reserve(arguments.size());
	for (const std::string& argument : arguments)
	{
		argumentViews.push_back(argument);
	}
	return argumentViews;
}
