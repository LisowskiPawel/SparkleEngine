#include "Process/Windows/ChildProcessWindowsArguments.h"
#include "Process/CommandLineUtils.h"
#include "Strings/StringUtils.h"

#include <stdexcept>

#if defined(_WIN32)
namespace Process
{
	std::wstring BuildWindowsChildCommandLine(const Process::ChildProcessRequest& request)
	{
		std::string commandLine = CommandLine::QuotePath(request.ExecutablePath);
		for (const std::string& argument : request.Arguments)
		{
			commandLine.push_back(' ');
			commandLine += CommandLine::QuoteArgument(argument);
		}
		if (commandLine.find('\0') != std::string::npos)
		{
			throw std::invalid_argument("Child process command line contains a NUL.");
		}
		return Strings::ToWide(commandLine);
	}

}
#endif
