#include "PCH.h"

#include "Console/ConsoleInputParser.h"
#include "Core/Public/Strings/StringUtils.h"

namespace ConsoleInputParser
{
	static bool ReadNextToken(std::string_view input, std::size_t& cursor, std::string& token, std::string& error)
	{
		token.clear();
		bool tokenActive = false;
		char quoteCharacter = '\0';
		while (cursor < input.size())
		{
			const char character = input[cursor++];
			if (character == '\\' && cursor < input.size())
			{
				tokenActive = true;
				token.push_back(input[cursor++]);
				continue;
			}
			if (quoteCharacter != '\0')
			{
				if (character == quoteCharacter)
				{
					quoteCharacter = '\0';
				}
				else
				{
					token.push_back(character);
				}
				continue;
			}
			if (character == '"' || character == '\'')
			{
				tokenActive = true;
				quoteCharacter = character;
				continue;
			}
			if (Strings::IsAsciiWhitespace(character))
			{
				if (tokenActive)
				{
					return true;
				}
				continue;
			}
			tokenActive = true;
			token.push_back(character);
		}
		if (quoteCharacter != '\0')
		{
			error = "unterminated quoted string";
			return false;
		}
		return tokenActive;
	}

	ConsoleParsedInput Parse(std::string_view input)
	{
		ConsoleParsedInput result;
		std::size_t cursor = 0;
		std::string error;
		if (ReadNextToken(input, cursor, result.CommandName, error))
		{
			std::string argument;
			while (ReadNextToken(input, cursor, argument, error))
			{
				result.Arguments.push_back(std::move(argument));
			}
		}
		if (!error.empty())
		{
			return {.Succeeded = false, .ErrorMessage = std::move(error)};
		}
		return result;
	}
}
