#include "Process/ChildProcessWindowsArguments.h"
#include "Process/ChildProcessWindows.h"
#include "Process/CommandLineUtils.h"

#include <algorithm>
#include <cwchar>

#if defined(_WIN32)
namespace Process
{
	static std::wstring Utf8ToWide(std::string_view text)
	{
		if (text.empty())
		{
			return {};
		}

		const int length = MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
		if (length <= 0)
		{
			return std::wstring(text.begin(), text.end());
		}

		std::wstring result(static_cast<std::size_t>(length), L'\0');
		MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), result.data(), length);
		return result;
	}

	std::wstring BuildWindowsChildCommandLine(const Process::ChildProcessRequest& request)
	{
		std::string commandLine = CommandLine::QuotePath(request.ExecutablePath);
		for (const std::string& argument : request.Arguments)
		{
			commandLine.push_back(' ');
			commandLine += CommandLine::QuoteArgument(argument);
		}
		return Utf8ToWide(commandLine);
	}

	std::vector<wchar_t> BuildWindowsChildEnvironment(const std::vector<Process::EnvironmentOverride>& overrides)
	{
		if (overrides.empty())
		{
			return {};
		}

		std::vector<std::wstring> entries;
		if (LPWCH environment = GetEnvironmentStringsW())
		{
			for (const wchar_t* current = environment; *current != L'\0'; current += std::wcslen(current) + 1)
			{
				entries.emplace_back(current);
			}
			FreeEnvironmentStringsW(environment);
		}
		for (const Process::EnvironmentOverride& overrideValue : overrides)
		{
			const std::wstring prefix = Utf8ToWide(overrideValue.Name) + L"=";
			const std::wstring replacement = prefix + Utf8ToWide(overrideValue.Value);
			auto entry = std::find_if(entries.begin(), entries.end(), [&prefix](const std::wstring& value) { return value.rfind(prefix, 0) == 0; });
			if (entry == entries.end())
			{
				entries.push_back(replacement);
			}
			else
			{
				*entry = replacement;
			}
		}

		std::sort(entries.begin(), entries.end());
		std::vector<wchar_t> block;
		for (const std::wstring& entry : entries)
		{
			block.insert(block.end(), entry.begin(), entry.end());
			block.push_back(L'\0');
		}
		block.push_back(L'\0');
		return block;
	}

}
#endif
