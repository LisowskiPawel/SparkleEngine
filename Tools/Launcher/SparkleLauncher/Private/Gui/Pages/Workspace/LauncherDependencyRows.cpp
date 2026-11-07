#include "Gui/Pages/Workspace/LauncherWorkspaceSetup.h"
#include "Gui/Widgets/LauncherOptionWidgets.h"
#include "Gui/Operations/LauncherOperationConfirmation.h"
#include <utility>

#include "Gui/Widgets/LauncherActionWidgets.h"
#include "Gui/Models/LauncherContentModel.h"
#include "Gui/Models/LauncherDependencyUiModel.h"
#include "Gui/Models/LauncherLevelUiModel.h"
#include "Gui/Models/LauncherOperationRequestFactory.h"
#include "Gui/App/LauncherSettings.h"

#include "ToolOutputPaths.h"
#include "SparkleLauncher/MaintenanceOperations.h"
#include "SparkleLauncher/SourceDependencyState.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

#include <filesystem>
#include <string>
#include <system_error>

namespace SparkleLauncher
{
	void LauncherWorkspaceSetup::AddSyncDependencies(QVBoxLayout& layout, bool bOptional)
	{
		const SparkleTools::ToolOutputPaths workspaceOutputs = SparkleTools::ResolveToolOutputPaths(m_workspaceRoot);
		const std::filesystem::path dependencyCachePath = workspaceOutputs.DependencyCacheRoot;
		for (const ThirdPartyDependencyUiEntry& dependency : GetTrackedThirdPartyDependencies())
		{
			if (dependency.bRequired == bOptional)
			{
				continue;
			}

			const ThirdPartyDependencyUiStatus status = BuildThirdPartyDependencyStatus(dependency, dependencyCachePath);
			const QString runId = m_sourceDependencyRunIds.value(dependency.Id);
			const bool bSyncing = !runId.isEmpty() && !m_cleaningSourceDependencyRunIds.contains(runId);
			const bool bCleaning = !runId.isEmpty() && m_cleaningSourceDependencyRunIds.contains(runId);
			QPushButton* actionButton = CreateSourceDependencyActionButton(dependency);

			QLabel* statusLabel = AddStatusRow(
			    layout,
			    dependency.Label,
			    bCleaning      ? QStringLiteral("Cleaning")
			        : bSyncing ? QStringLiteral("Syncing")
			                  : status.Text,
			    status.Detail,
			    bCleaning || bSyncing ? QStringLiteral("running") : status.State,
			    actionButton);

			ApplySourceDependencyRowState(dependency, *statusLabel, *actionButton);
			m_sourceDependencyStatusLabels.insert(dependency.Id, statusLabel);
			m_sourceDependencyActionButtons.insert(dependency.Id, actionButton);
		}
	}

	QPushButton* LauncherWorkspaceSetup::CreateSourceDependencyActionButton(const ThirdPartyDependencyUiEntry& dependency)
	{
		QPushButton* button = new QPushButton(&m_dialogParent);
		emit FocusableWidgetCreated(button);

		connect(
		    button,
		    &QPushButton::clicked,
		    this,
		    [this, dependency, button]()
		    {
			    const QString actionIntent = button->property("ActionIntent").toString();
			    if (actionIntent == QStringLiteral("clean"))
			    {
				    CleanSourceDependency(dependency);
			    }
			    else if (actionIntent == QStringLiteral("sync"))
			    {
				    SyncSourceDependency(dependency);
			    }
		    });

		return button;
	}

	void LauncherWorkspaceSetup::ApplySourceDependencyRowState(const ThirdPartyDependencyUiEntry& dependency, QLabel& statusLabel, QPushButton& button)
	{
		const SparkleTools::ToolOutputPaths workspaceOutputs = SparkleTools::ResolveToolOutputPaths(m_workspaceRoot);
		const std::filesystem::path dependencyCachePath = workspaceOutputs.DependencyCacheRoot;
		const ThirdPartyDependencyUiStatus status = BuildThirdPartyDependencyStatus(dependency, dependencyCachePath);
		const QString runId = m_sourceDependencyRunIds.value(dependency.Id);
		const bool bCleaning = !runId.isEmpty() && m_cleaningSourceDependencyRunIds.contains(runId);
		const bool bSyncing = !runId.isEmpty() && !bCleaning;
		const SyncItemState state = bSyncing ? SyncItemState::Syncing : status.bSynced ? SyncItemState::Synced : SyncItemState::Missing;

		ApplyInlineStatusLabel(statusLabel, bCleaning ? QStringLiteral("Cleaning") : bSyncing ? QStringLiteral("Syncing") : status.Text, bCleaning || bSyncing ? QStringLiteral("running") : status.State);
		ApplySyncActionButtonState(button, state, dependency.Label);
		if (bCleaning)
		{
			button.setText("Cleaning...");
			button.setProperty("ActionIntent", "none");
			button.setEnabled(false);
			button.setAccessibleName("Cleaning " + dependency.Label);
			button.setToolTip("Clean is in progress.");
		}
	}

	void LauncherWorkspaceSetup::RefreshSourceDependencyRows()
	{
		for (const ThirdPartyDependencyUiEntry& dependency : GetTrackedThirdPartyDependencies())
		{
			QLabel* statusLabel = m_sourceDependencyStatusLabels.value(dependency.Id, nullptr);
			QPushButton* actionButton = m_sourceDependencyActionButtons.value(dependency.Id, nullptr);
			if (statusLabel != nullptr && actionButton != nullptr)
			{
				ApplySourceDependencyRowState(dependency, *statusLabel, *actionButton);
			}
		}
	}

	void LauncherWorkspaceSetup::SyncSourceDependency(const ThirdPartyDependencyUiEntry& dependency)
	{
		LauncherOperationRequest request = BuildLauncherOperationRequest(m_workspaceRoot, m_contentModel, m_settings, "workspace.sync-code");
		request.SourceDependencyId = dependency.Id;
		m_startOperation(std::move(request), "Sync " + dependency.Label);
	}

	void LauncherWorkspaceSetup::CleanSourceDependency(const ThirdPartyDependencyUiEntry& dependency)
	{
		const SourceDependencyEntry* sourceDependency = FindSourceDependency(dependency.Id.toStdString());
		if (sourceDependency == nullptr)
		{
			return;
		}

		LauncherOperationRequest request = BuildLauncherOperationRequest(m_workspaceRoot, m_contentModel, m_settings, "workspace.clean");
		const SparkleTools::ToolOutputPaths workspaceOutputs = SparkleTools::ResolveToolOutputPaths(m_workspaceRoot);
		const std::filesystem::path dependencyCachePath = workspaceOutputs.DependencyCacheRoot;
		for (const std::filesystem::path& path : GetSourceDependencyCachePaths(*sourceDependency, dependencyCachePath))
		{
			std::error_code errorCode;
			if (!std::filesystem::exists(path, errorCode) || errorCode)
			{
				continue;
			}
			request.CleanTargets.push_back({dependency.Label + " cache", QString::fromStdString(path.string()), "Generated source dependency cache."});
		}
		if (request.CleanTargets.empty() || !ConfirmLauncherRunRequest(m_dialogParent, request))
		{
			return;
		}

		const QString runId = m_startOperation(std::move(request), "Clean " + dependency.Label);
		m_sourceDependencyRunIds.insert(dependency.Id, runId);
		m_cleaningSourceDependencyRunIds.insert(runId);
		RefreshSourceDependencyRows();
	}

	void LauncherWorkspaceSetup::TrackSourceDependencyRun(const LauncherOperationRequest& request, const QString& runId)
	{
		if (request.OperationId != "workspace.sync-code")
		{
			return;
		}

		if (!request.SourceDependencyId.isEmpty())
		{
			m_sourceDependencyRunIds.insert(request.SourceDependencyId, runId);
			RefreshSourceDependencyRows();
			return;
		}

		for (const ThirdPartyDependencyUiEntry& dependency : GetTrackedThirdPartyDependencies())
		{
			if (!dependency.bEnabled)
			{
				continue;
			}
			m_sourceDependencyRunIds.insert(dependency.Id, runId);
		}
		RefreshSourceDependencyRows();
	}

}
