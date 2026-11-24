#pragma once

#include "Gui/Style/LauncherIconLibrary.h"

#include <QtCore/QString>
#include <QtCore/QVector>

namespace SparkleLauncher
{
	struct LauncherWorkflowDefinition
	{
		QString OperationId;
		QString DisplayName;
		LauncherIcon Icon;
	};

	QString LauncherScenesOperationId();
	QVector<LauncherWorkflowDefinition> CreateLauncherWorkflowCatalog();
	QString LauncherOperationDisplayNameOverride(const QString& operationId);
}
