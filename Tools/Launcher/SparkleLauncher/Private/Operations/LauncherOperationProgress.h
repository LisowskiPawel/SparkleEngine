#pragma once

#include "Core/Public/Process/ChildProcess.h"

#include <cstddef>
#include <string_view>

namespace SparkleLauncher
{
	void ReportOperationProgress(const Process::ChildProcessOutputCallback& outputCallback, std::string_view phase, std::size_t completed = 0, std::size_t total = 0);
}
