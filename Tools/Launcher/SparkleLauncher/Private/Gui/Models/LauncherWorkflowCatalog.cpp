#include "LauncherWorkflowCatalog.h"

#include "LauncherUiModel.h"

namespace SparkleLauncher
{
	QString LauncherScenesOperationId()
	{
		return "levels.browse";
	}

	QVector<LauncherWorkflowDefinition> CreateLauncherWorkflowCatalog()
	{
		return {
		    {LauncherWorkflowPageKind::Scenes, {LauncherScenesOperationId()}},
		    {LauncherWorkflowPageKind::Sync, {"workspace.sync-code"}},
		    {LauncherWorkflowPageKind::Build, {"workspace.build"}},
		    {LauncherWorkflowPageKind::Cook, {"cook.workspace"}},
		    {LauncherWorkflowPageKind::Clean, {"workspace.clean"}},
		};
	}

	QString LauncherOperationDisplayNameOverride(const QString& operationId)
	{
		return LauncherUiModelForOperation(operationId).DisplayName;
	}

	QString LauncherOperationImpactText(const QString& operationId)
	{
		return LauncherUiModelForOperation(operationId).ImpactText;
	}

	QString LauncherWorkflowPrimaryVerb(const QString& operationId)
	{
		const QString primaryVerb = LauncherUiModelForOperation(operationId).PrimaryVerb;
		return primaryVerb.isEmpty() ? QString("Run") : primaryVerb;
	}
}
