#pragma once

#include "Core/Public/Console/ConsoleOutput.h"

#include <cstddef>
#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

struct ConsoleAutocompleteRequest final
{
	std::string_view CommandName;
	std::span<const std::string_view> Arguments;
	std::string_view CurrentToken;
};

using ConsoleCommandCallback = std::function<ConsoleCommandResult(std::span<const std::string_view>)>;
using ConsoleAutocompleteCallback = std::function<std::vector<std::string>(const ConsoleAutocompleteRequest&)>;

struct ConsoleCommandDescriptor final
{
	std::string Name;
	std::string Help;
	std::string ArgumentSyntax;
	ConsoleCommandCallback Execute;
	ConsoleAutocompleteCallback Complete;
};

class ConsoleCommandRegistry final
{
public:
	bool Register(ConsoleCommandDescriptor descriptor);

	const ConsoleCommandDescriptor* Find(std::string_view commandName) const;

	const std::vector<ConsoleCommandDescriptor>& GetCommands() const noexcept { return m_commands; }

	ConsoleCommandResult ExecuteLine(std::string_view input) const;
	std::vector<std::string> CompleteLine(std::string_view input) const;

private:
	static std::vector<std::string_view> BuildArgumentViews(const std::vector<std::string>& arguments);

	std::vector<ConsoleCommandDescriptor> m_commands;
	std::unordered_map<std::string, std::size_t> m_commandIndicesByName;
};
