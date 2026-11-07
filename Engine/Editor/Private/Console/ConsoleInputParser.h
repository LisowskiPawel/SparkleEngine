#pragma once

#include <string>
#include <string_view>
#include <vector>

struct ConsoleParsedInput final
{
	bool Succeeded = true;
	std::string CommandName;
	std::vector<std::string> Arguments;
	std::string ErrorMessage;

	bool IsEmpty() const noexcept { return Succeeded && CommandName.empty(); }
};

namespace ConsoleInputParser
{
	ConsoleParsedInput Parse(std::string_view input);
}
