#pragma once

#include "SparkleLauncher/MaintenanceOperations.h"

#include <QtCore/QString>

#include <filesystem>
#include <optional>
#include <vector>

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
	std::optional<std::vector<CleanScope>> ParseCleanScopeSelection(const QString& scopeSelection);
	QString CleanScopeSelectionError(const QString& scopeSelection);
	bool CleanScopeSelectionRequiresContent(const QString& scopeSelection);
}
