#include "PCH.h"

#include "Console/ConsoleInputParser.h"
#include "Core/Public/Strings/StringUtils.h"

namespace ConsoleInputParser
{
	static bool ReadNextToken(std::string_view input, std::size_t& cursor, std::string& token, std::string& error)
	{
		token.clear();
		bool bTokenActive = false;
		char quoteCharacter = '\0';
		while (cursor < input.size())
		{
			const char character = input[cursor++];
			if (character == '\\' && cursor < input.size())
			{
				bTokenActive = true;
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
				bTokenActive = true;
				quoteCharacter = character;
				continue;
			}
			if (Strings::IsAsciiWhitespace(character))
			{
				if (bTokenActive)
				{
					return true;
				}
				continue;
			}
			bTokenActive = true;
			token.push_back(character);
		}
		if (quoteCharacter != '\0')
		{
			error = "unterminated quoted string";
			return false;
		}
		return bTokenActive;
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
			return {.bSucceeded = false, .ErrorMessage = std::move(error)};
		}
		return result;
	}
}
