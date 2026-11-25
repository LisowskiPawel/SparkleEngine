#include "Gui/Pages/Workspace/LauncherWorkspaceSetup.h"
#include "Gui/Widgets/LauncherOptionWidgets.h"
#include "Gui/Operations/LauncherOperationConfirmation.h"
#include <utility>

#include "Gui/Widgets/LauncherActionWidgets.h"
#include "Gui/Models/LauncherContentModel.h"
#include "Gui/Models/LauncherOperationRequestFactory.h"
#include "Gui/App/LauncherSettings.h"

#include "ToolOutputPaths.h"
#include "SparkleLauncher/MaintenanceOperations.h"
#include "SparkleLauncher/SourceDependencyState.h"

#include <QtGui/QAction>
#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

#include <filesystem>
#include <string>
#include <system_error>

namespace SparkleLauncher
{
	struct SourceDependencyStatus final
	{
		QString Text;
		QString Detail;
		QString State;
		bool bHasCache = false;
	};

	static QString SourceDependencyDetails(const SourceDependencyEntry& dependency)
	{
		QString details = QString::fromStdString(dependency.Version) + "\n" + QString::fromStdString(dependency.Purpose);
		if (!dependency.ExpectedGitRevision.empty())
		{
			details += "\nSource pin: " + QString::fromStdString(dependency.ExpectedGitRevision);
		}
		return details;
	}

	static SourceDependencyStatus InspectDependencyStatus(const SourceDependencyEntry& dependency, const std::filesystem::path& dependencyCachePath)
	{
		const SourceDependencyValidation validation = InspectSourceDependencyAvailability(dependency, dependencyCachePath);
		const QString version = QString::fromStdString(dependency.Version).section('+', 0, 0);
		const QString detail = validation.FailureReason.empty() ? version : QString::fromStdString(validation.FailureReason);
		std::error_code errorCode;
		const bool bHasCache = std::filesystem::exists(validation.CachePath, errorCode);
		const QString unavailableState = !validation.FailureReason.empty() ? "Unavailable" : (bHasCache ? "Incomplete" : "Missing");
		return validation.bReady ? SourceDependencyStatus{"Available", detail, "ok", bHasCache} : SourceDependencyStatus{unavailableState, detail, "warning", bHasCache};
	}

	void LauncherWorkspaceSetup::AddSyncDependencies(QVBoxLayout& layout, bool bOptional)
	{
		for (const SourceDependencyEntry& dependency : GetSourceDependencies())
		{
			if (dependency.bRequired == bOptional)
			{
				continue;
			}

			const QString id = QString::fromStdString(dependency.Id);
			const QString label = QString::fromStdString(dependency.Label);
			auto* button = new QPushButton(layout.parentWidget());
			ApplyStatusActionButtonPresentation(*button, "Sync", "warning");
			auto* statusLabel = AddStatusRow(layout, label, QString(), QString::fromStdString(dependency.Version), "neutral", button);
			auto* row = statusLabel->parentWidget();
			auto* detailLabel = row->findChild<QLabel*>("StatusDetail");
			m_sourceDependencyRows.push_back({id, statusLabel, detailLabel, button});
			connect(button, &QPushButton::clicked, this, [this, id]() { ActivateSourceDependency(id); });
			auto* copy = row->addAction(
			    "Copy dependency details",
			    QKeySequence::Copy,
			    row,
			    [row, label, statusLabel]() { QGuiApplication::clipboard()->setText(label + "\n" + statusLabel->text() + "\n" + row->toolTip()); });
			copy->setShortcutContext(Qt::WidgetWithChildrenShortcut);
			row->setContextMenuPolicy(Qt::ActionsContextMenu);
			emit FocusableWidgetCreated(button);
		}
		RefreshSourceDependencyRows();
	}

	void LauncherWorkspaceSetup::RefreshSourceDependencyRows()
	{
		const SparkleTools::ToolOutputPaths workspaceOutputs = SparkleTools::ResolveToolOutputPaths(m_workspaceRoot);
		for (const SourceDependencyRow& row : m_sourceDependencyRows)
		{
			if (row.StatusLabel == nullptr || row.ActionButton == nullptr)
			{
				continue;
			}
			const SourceDependencyEntry* dependency = FindSourceDependency(row.Id.toStdString());
			if (dependency == nullptr)
			{
				continue;
			}

			const QString runId = m_sourceDependencyRunIds.value(row.Id);
			const bool bActive = !runId.isEmpty();
			const bool bCleaning = bActive && m_cleaningSourceDependencyRunIds.contains(runId);
			const auto status = bActive ? SourceDependencyStatus{} : InspectDependencyStatus(*dependency, workspaceOutputs.DependencyCacheRoot);
			const QString statusText = bActive ? (bCleaning ? "Cleaning" : "Syncing") : status.Text;
			const QString action = bActive ? statusText + "..." : status.bHasCache ? "Clean" : "Sync";
			const QString state = bActive ? QStringLiteral("running") : status.bHasCache ? QStringLiteral("ok") : QStringLiteral("warning");
			ApplyInlineStatusLabel(*row.StatusLabel, statusText, bActive ? QStringLiteral("running") : status.State);
			ApplyStatusActionButtonPresentation(*row.ActionButton, action, state);
			row.ActionButton->setEnabled(!bActive);
			row.ActionButton->setAccessibleName(action + " " + QString::fromStdString(dependency->Label));
			const QString detail = bActive ? QString::fromStdString(dependency->Version).section('+', 0, 0) : status.Detail;
			const QString tooltip = SourceDependencyDetails(*dependency) + "\n" + detail;
			row.StatusLabel->parentWidget()->setToolTip(tooltip);
			row.ActionButton->setToolTip(tooltip);
			row.ActionButton->setAccessibleDescription(statusText + ". " + tooltip);
			if (row.DetailLabel != nullptr)
			{
				row.DetailLabel->setText(detail);
			}
		}
	}

	void LauncherWorkspaceSetup::ActivateSourceDependency(const QString& id)
	{
		const SourceDependencyEntry* dependency = FindSourceDependency(id.toStdString());
		if (dependency == nullptr || m_sourceDependencyRunIds.contains(id))
		{
			return;
		}
		const auto outputs = SparkleTools::ResolveToolOutputPaths(m_workspaceRoot);
		const auto status = InspectDependencyStatus(*dependency, outputs.DependencyCacheRoot);
		if (status.bHasCache)
		{
			CleanSourceDependency(*dependency);
		}
		else
		{
			SyncSourceDependency(*dependency);
		}
	}

	void LauncherWorkspaceSetup::SyncSourceDependency(const SourceDependencyEntry& dependency)
	{
		LauncherOperationRequest request = BuildLauncherOperationRequest(m_workspaceRoot, m_contentModel, m_settings, "workspace.sync-code");
		std::get<BuildWorkspaceOperationRequest>(request.Inputs).SourceDependencyId = dependency.Id;
		m_startOperation(std::move(request), "Sync " + QString::fromStdString(dependency.Label));
	}

	void LauncherWorkspaceSetup::CleanSourceDependency(const SourceDependencyEntry& dependency)
	{
		LauncherOperationRequest request = BuildLauncherOperationRequest(m_workspaceRoot, m_contentModel, m_settings, "workspace.clean");
		auto& inputs = std::get<MaintenanceOperationRequest>(request.Inputs);
		const SparkleTools::ToolOutputPaths workspaceOutputs = SparkleTools::ResolveToolOutputPaths(m_workspaceRoot);
		const std::filesystem::path dependencyCachePath = workspaceOutputs.DependencyCacheRoot;
		for (const std::filesystem::path& path : GetSourceDependencyCachePaths(dependency, dependencyCachePath))
		{
			std::error_code errorCode;
			if (!std::filesystem::exists(path, errorCode) || errorCode)
			{
				continue;
			}

			inputs.RequestedCleanTargets.push_back({dependency.Label + " cache", path, "Generated source dependency cache."});
		}
		if (inputs.RequestedCleanTargets.empty())
		{
			return;
		}
		if (!ConfirmLauncherRunRequest(m_dialogParent, request))
		{
			return;
		}

		const QString runId = m_startOperation(std::move(request), "Clean " + QString::fromStdString(dependency.Label));
		m_sourceDependencyRunIds.insert(QString::fromStdString(dependency.Id), runId);
		m_cleaningSourceDependencyRunIds.insert(runId);
		RefreshSourceDependencyRows();
	}

	void LauncherWorkspaceSetup::TrackSourceDependencyRun(const LauncherOperationRequest& request, const QString& runId)
	{
		if (request.OperationId != "workspace.sync-code")
		{
			return;
		}

		const auto& inputs = std::get<BuildWorkspaceOperationRequest>(request.Inputs);
		if (!inputs.SourceDependencyId.empty())
		{
			m_sourceDependencyRunIds.insert(QString::fromStdString(inputs.SourceDependencyId), runId);
			RefreshSourceDependencyRows();
			return;
		}

		for (const SourceDependencyEntry& dependency : GetSourceDependencies())
		{
			if (!dependency.bEnabled)
			{
				continue;
			}
			m_sourceDependencyRunIds.insert(QString::fromStdString(dependency.Id), runId);
		}
		RefreshSourceDependencyRows();
	}

}
