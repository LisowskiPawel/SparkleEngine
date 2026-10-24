#include "LauncherOperationProgress.h"

#include "ToolWorkProgress.h"

#include <cstddef>
#include <string_view>

namespace SparkleLauncher
{
	void ReportOperationProgress(const Process::ChildProcessOutputCallback& outputCallback, std::string_view phase, std::size_t completed, std::size_t total)
	{
		if (outputCallback)
		{
			outputCallback(ToolWorkProgressProtocol::Format(phase, completed, total));
		}
	}
}
