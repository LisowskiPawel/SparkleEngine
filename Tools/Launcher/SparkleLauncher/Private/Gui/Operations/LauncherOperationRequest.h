#pragma once

#include "SparkleLauncher/BuildWorkspaceOperations.h"
#include "SparkleLauncher/CookOperations.h"
#include "SparkleLauncher/LevelOperations.h"
#include "SparkleLauncher/LevelRunOperations.h"
#include "SparkleLauncher/MaintenanceOperations.h"

#include <QtCore/QString>
#include <variant>

namespace SparkleLauncher
{
	using LauncherOperationInputs = std::variant<BuildWorkspaceOperationRequest, LevelOperationRequest, LevelRunOperationRequest, CookOperationRequest, MaintenanceOperationRequest>;

	struct LauncherOperationRequest
	{
		QString RunId;
		QString OperationId;
		LauncherOperationInputs Inputs;
	};
}
