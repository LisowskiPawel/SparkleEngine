#pragma once

#include "Gui/Operations/LauncherOperationRequest.h"
#include "SparkleLauncher/OperationModel.h"
#include "SparkleLauncher/ProcessRunner.h"

class TaskExecutionContext;

namespace SparkleLauncher
{
	OperationRecord ExecuteLauncherOperation(
	    const LauncherOperationRequest& request,
	    IProcessRunner& processRunner,
	    TaskExecutionContext& context,
	    const Process::ChildProcessOutputCallback& outputCallback);
}
