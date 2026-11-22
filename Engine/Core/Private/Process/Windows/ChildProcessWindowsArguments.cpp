#include "Process/Windows/ChildProcessWindowsArguments.h"
#include "Process/Windows/ChildProcessWindows.h"
#include "Process/CommandLineUtils.h"

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

}
#endif
