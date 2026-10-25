#include "LauncherOperationProgress.h"

#include "ToolWorkProgress.h"

#include <cstddef>
#include <string>
#include <string_view>

namespace SparkleLauncher
{
	void ReportOperationProgress(const Process::ChildProcessOutputCallback& outputCallback, std::string_view phase, std::size_t completed, std::size_t total)
	{
		if (outputCallback)
		{
			const std::string record = FormatToolWorkProgress(phase, completed, total);
			outputCallback(record);
		}
	}
}
