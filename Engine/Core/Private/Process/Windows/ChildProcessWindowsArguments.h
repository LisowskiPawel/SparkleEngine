#pragma once

#include "Process/ChildProcess.h"

#if defined(_WIN32)
namespace Process
{
	std::wstring BuildWindowsChildCommandLine(const ChildProcessRequest& request);
	std::vector<wchar_t> BuildWindowsChildEnvironment(const std::vector<EnvironmentOverride>& overrides);
}
#endif
