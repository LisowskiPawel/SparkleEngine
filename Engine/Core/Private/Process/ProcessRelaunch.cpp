#include "PCH.h"

#include "Core/Public/Process/ChildProcess.h"

#include "Core/Public/Paths/FileSystemPaths.h"

#include <filesystem>
#include <string>
#include <system_error>

namespace Process
{
	bool RelaunchCurrentProcess()
	{
#if defined(_WIN32)
		const std::filesystem::path executable = Filesystem::GetExecutablePath();
		std::error_code error;
		const std::filesystem::path workingDirectory = std::filesystem::current_path(error);
		if (executable.empty() || error)
		{
			return false;
		}

		std::wstring commandLine(GetCommandLineW());
		STARTUPINFOW startup{};
		startup.cb = sizeof(startup);
		PROCESS_INFORMATION child{};
		if (!CreateProcessW(executable.c_str(), commandLine.data(), nullptr, nullptr, FALSE, 0, nullptr, workingDirectory.c_str(), &startup, &child))
		{
			return false;
		}
		CloseHandle(child.hThread);
		CloseHandle(child.hProcess);
		return true;
#else
		return false;
#endif
	}
}
