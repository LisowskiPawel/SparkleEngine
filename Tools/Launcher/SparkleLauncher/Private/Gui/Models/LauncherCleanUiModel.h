#pragma once

#include "Gui/Operations/LauncherOperationRequest.h"
#include "SparkleLauncher/MaintenanceOperations.h"

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVector>

#include <filesystem>

namespace SparkleLauncher
{
	struct CleanScopeUiOption
	{
		QString Label;
		QString Value;
		QString Detail;
		QString Preview;
		QString Group;
	};

	std::filesystem::path ResolveCleanScopePreviewPath(const std::filesystem::path& workspaceRoot, const QString& projectId, const QString& scope);

	QString CleanScopeDisplayName(const QString& scopeValue);
	QString CleanScopeSelectionError(const QString& scopeSelection);
	bool CleanScopeSelectionRequiresContent(const QString& scopeSelection);

	QVector<LauncherCleanTarget> BuildActionSpecificCleanTargets(const ActionCleanTargetRequest& request);
}
