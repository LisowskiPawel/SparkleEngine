#pragma once

#include "Operations/LauncherOperationExecution.h"

#include <QtCore/QString>

namespace SparkleLauncher
{
	struct LauncherOperationRequest
	{
		QString RunId;
		QString OperationId;
		LauncherOperationInputs Inputs;
	};
}
