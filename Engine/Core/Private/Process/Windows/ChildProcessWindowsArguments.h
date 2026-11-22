#pragma once

#include "Process/ChildProcess.h"

#if defined(_WIN32)
namespace Process
{
	std::wstring BuildWindowsChildCommandLine(const ChildProcessRequest& request);
}
#endif
