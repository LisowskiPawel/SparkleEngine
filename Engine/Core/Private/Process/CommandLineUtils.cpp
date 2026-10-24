#include "PCH.h"

#include "Core/Public/Process/CommandLineUtils.h"

#include <cwctype>
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Strings/StringUtils.h"
#include <memory>
#if defined(_WIN32)
  #include <Windows.h>
  #include <shellapi.h>
#endif

namespace CommandLine
{
	std::vector<std::string> ReadProcessArguments()
	{
#if defined(_WIN32)
		int count = 0;
		std::unique_ptr<wchar_t*, decltype(&LocalFree)> arguments(CommandLineToArgvW(GetCommandLineW(), &count), &LocalFree);
		if (!arguments)
		{
			throw Diagnostics::Error("Cannot read process arguments.");
		}
		std::vector<std::string> values;
		values.reserve(static_cast<std::size_t>(count));
		for (int index = 0; index < count; ++index)
		{
			values.push_back(Strings::ToNarrow(arguments.get()[index]));
		}
		return values;
#else
		throw Diagnostics::Error("Process argument discovery is unavailable on this platform.");
#endif
	}

	std::wstring_view ReadToken(std::wstring_view commandLine, std::size_t& offset) noexcept
	{
		while (offset < commandLine.size() && std::iswspace(commandLine[offset]))
		{
			++offset;
		}

		if (offset >= commandLine.size())
		{
			return {};
		}

		const std::size_t tokenStart = offset;
		if (commandLine[offset] == L'"')
		{
			++offset;
			const std::size_t quotedStart = offset;
			while (offset < commandLine.size() && commandLine[offset] != L'"')
			{
				++offset;
			}
			const std::size_t quotedEnd = offset;
			if (offset < commandLine.size())
			{
				++offset;
			}
			return commandLine.substr(quotedStart, quotedEnd - quotedStart);
		}

		while (offset < commandLine.size() && !std::iswspace(commandLine[offset]))
		{
			++offset;
		}
		return commandLine.substr(tokenStart, offset - tokenStart);
	}

	std::string QuoteArgument(std::string_view text)
	{
		std::string quoted;
		quoted.reserve(text.size() + 2);
		quoted.push_back('"');
		std::size_t backslashes = 0;
		for (const char character : text)
		{
			if (character == '\\')
			{
				++backslashes;
				continue;
			}

			quoted.append(character == '"' ? backslashes * 2 + 1 : backslashes, '\\');
			backslashes = 0;
			quoted.push_back(character);
		}
		quoted.append(backslashes * 2, '\\');
		quoted.push_back('"');
		return quoted;
	}

	std::string QuotePath(const std::filesystem::path& path)
	{
		return QuoteArgument(Strings::ToNarrow(path.wstring()));
	}
}
