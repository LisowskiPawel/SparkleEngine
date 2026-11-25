#include "Gui/Models/LauncherWorkflowCatalog.h"

namespace SparkleLauncher
{
	QString LauncherScenesOperationId()
	{
		return "levels.browse";
	}

	QVector<LauncherWorkflowDefinition> CreateLauncherWorkflowCatalog()
	{
		return {
		    {LauncherScenesOperationId(), "Quick Start", LauncherIcon::Start},
		    {"workspace.sync-code", "Sync", LauncherIcon::Sync},
		    {"workspace.build", "Build", LauncherIcon::Build},
		    {"cook.workspace", "Cook", LauncherIcon::Cook},
		    {"workspace.clean", "Clean", LauncherIcon::Clean},
		};
	}

	QString LauncherOperationDisplayNameOverride(const QString& operationId)
	{
		if (operationId == LauncherScenesOperationId())
		{
			return "Quick Start";
		}
		if (operationId == "cook.tools.prepare")
		{
			return "Build Cook Tools";
		}
		if (operationId == "cook.assets")
		{
			return "Cook Scene Assets";
		}
		return {};
	}

}
