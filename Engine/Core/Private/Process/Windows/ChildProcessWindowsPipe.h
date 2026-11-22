#pragma once
#include "Process/Windows/ChildProcessWindows.h"

#if defined(_WIN32)
namespace Process
{
	bool CreateChildOutputPipe(WindowsProcessHandle& readPipe, WindowsProcessHandle& writePipe, ChildProcessResult& result);
}
#endif
