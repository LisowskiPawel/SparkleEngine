#include "Gui/Models/LauncherCleanUiModel.h"

#include "Core/Public/Paths/ProductUserStatePaths.h"
#include "ToolOutputPaths.h"
#include "SparkleLauncher/MaintenanceOperations.h"

#include <QtCore/QRegularExpression>
#include <QtCore/QStringList>

#include <algorithm>
#include <optional>
#include <vector>

namespace SparkleLauncher
{
	std::filesystem::path ResolveCleanScopePreviewPath(const std::filesystem::path& workspaceRoot, const QString& projectId, const QString& scope)
	{
		const SparkleTools::ToolOutputPaths outputs = SparkleTools::ResolveToolOutputPaths(workspaceRoot);
		CleanScope cleanScope = CleanScope::CookedOutputs;
		if (!TryParseCleanScope(scope.toStdString(), cleanScope))
		{
			return workspaceRoot;
		}

		switch (cleanScope)
		{
			case CleanScope::CookedOutputs:
				return outputs.CookedProjectDirectory(projectId.toStdString());
			case CleanScope::BuildTree:
				return outputs.BuildRoot;
			case CleanScope::ArtifactOutputs:
				return outputs.ArtifactRoot;
			case CleanScope::WorkspaceState:
			case CleanScope::PristineGeneratedWorkspace:
				return workspaceRoot;
			case CleanScope::ThirdPartyDependencyCache:
				return outputs.DependencyCacheRoot;
			case CleanScope::Logs:
				return Filesystem::ResolveDevelopmentProductUserStatePaths(workspaceRoot, projectId.toStdString()).LogsRoot;
		}
		return workspaceRoot;
	}

	QString CleanScopeDisplayName(const QString& scopeValue)
	{
		CleanScope scope = CleanScope::CookedOutputs;
		if (!TryParseCleanScope(scopeValue.toStdString(), scope))
		{
			return scopeValue;
		}

		switch (scope)
		{
			case CleanScope::CookedOutputs:
				return "Cooked Content";
			case CleanScope::BuildTree:
				return "Build Outputs";
			case CleanScope::ArtifactOutputs:
				return "Generated Artifacts";
			case CleanScope::WorkspaceState:
				return "IDE And Workspace State";
			case CleanScope::ThirdPartyDependencyCache:
				return "Source Dependency Cache";
			case CleanScope::Logs:
				return "Log Files";
			case CleanScope::PristineGeneratedWorkspace:
				return "Clean All";
		}
		return scopeValue;
	}

	static std::optional<std::vector<CleanScope>> ParseCleanScopeSelection(const QString& scopeSelection)
	{
		const QStringList selectedScopes = scopeSelection.split(QRegularExpression("[,;\\n]"), Qt::SkipEmptyParts);
		if (selectedScopes.empty())
		{
			return std::nullopt;
		}

		std::vector<CleanScope> scopes;
		for (const QString& selectedScope : selectedScopes)
		{
			CleanScope scope = CleanScope::CookedOutputs;
			if (!TryParseCleanScope(selectedScope.trimmed().toStdString(), scope))
			{
				return std::nullopt;
			}
			scopes.push_back(scope);
		}
		return scopes;
	}

	QString CleanScopeSelectionError(const QString& scopeSelection)
	{
		if (scopeSelection.trimmed().isEmpty())
		{
			return "Select at least one generated-data category to clean.";
		}
		if (!ParseCleanScopeSelection(scopeSelection).has_value())
		{
			return "The saved clean selection is invalid. Select the generated-data categories again.";
		}
		return {};
	}

	bool CleanScopeSelectionRequiresContent(const QString& scopeSelection)
	{
		const std::optional<std::vector<CleanScope>> scopes = ParseCleanScopeSelection(scopeSelection);
		return scopes.has_value() && std::any_of(scopes->begin(), scopes->end(), CleanScopeRequiresContent);
	}

	QVector<LauncherCleanTarget> BuildActionSpecificCleanTargets(const ActionCleanTargetRequest& request)
	{
		QVector<LauncherCleanTarget> targets;
		for (const MaintenanceCleanPathSpec& path : BuildActionCleanPaths(request))
		{
			targets.push_back({QString::fromStdString(path.DisplayName), QString::fromStdString(path.Path.string()), QString::fromStdString(path.Detail)});
		}
		return targets;
	}
}
