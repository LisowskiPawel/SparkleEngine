#include "Gui/Pages/Workspace/LauncherWorkspaceSetup.h"
#include "Gui/Widgets/LauncherOptionWidgets.h"
#include "Gui/Operations/LauncherOperationConfirmation.h"
#include <utility>

#include "Gui/Style/LauncherUiDesign.h"
#include "Gui/Models/LauncherContentModel.h"
#include "Gui/Models/LauncherOperationRequestFactory.h"
#include "Gui/App/LauncherSettings.h"

#include "ToolOutputPaths.h"
#include "SparkleLauncher/MaintenanceOperations.h"
#include "SparkleLauncher/SourceDependencyState.h"

#include <QtGui/QAction>
#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>
#include <QtGui/QColor>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QToolButton>
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

	static QString SourceDependencyPurpose(const SourceDependencyEntry& dependency)
	{
		const std::string purpose = dependency.ExpectedGitRevision.empty() ? dependency.Purpose : dependency.Purpose + "\nSource pin: " + dependency.ExpectedGitRevision;
		return QString::fromStdString(purpose);
	}

	static SourceDependencyStatus InspectDependencyStatus(const SourceDependencyEntry& dependency, const std::filesystem::path& dependencyCachePath)
	{
		const SourceDependencyValidation validation = InspectSourceDependencyAvailability(dependency, dependencyCachePath);
		const QString purpose = SourceDependencyPurpose(dependency);
		const QString version = QString::fromStdString(dependency.Version);
		const QString description = purpose.isEmpty() ? version : QStringLiteral("%1 \u00b7 %2").arg(version, purpose);
		const QString detail = validation.FailureReason.empty() ? description : QString::fromStdString(validation.FailureReason);
		std::error_code errorCode;
		const bool bHasCache = std::filesystem::exists(validation.CachePath, errorCode);
		const QString unavailableState = !validation.FailureReason.empty() ? "Unavailable" : (bHasCache ? "Incomplete" : "Missing");
		return validation.bReady ? SourceDependencyStatus{"Available", detail, "ok", bHasCache} : SourceDependencyStatus{unavailableState, detail, "warning", bHasCache};
	}

	void LauncherWorkspaceSetup::AddSyncDependencies(QVBoxLayout& layout, bool bOptional)
	{
		auto* view = new QTreeWidget(layout.parentWidget());
		view->setObjectName("SourceDependencyView");
		view->setAccessibleName(bOptional ? "Optional source dependencies" : "Source dependencies");
		view->setAccessibleDescription("Select a dependency and use its action button or press Enter to sync or clean. Ctrl+C copies its details.");
		view->setHeaderLabels({"Dependency", "Status", "Available action", "Details"});
		view->setRootIsDecorated(false);
		view->setSelectionMode(QAbstractItemView::SingleSelection);
		view->setEditTriggers(QAbstractItemView::NoEditTriggers);
		view->setMinimumHeight(180);
		view->setColumnWidth(0, 190);
		view->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
		view->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
		for (const SourceDependencyEntry& dependency : GetSourceDependencies())
		{
			if (dependency.bRequired == bOptional)
			{
				continue;
			}

			auto* item = new QTreeWidgetItem(view, {QString::fromStdString(dependency.Label)});
			item->setData(0, Qt::UserRole, QString::fromStdString(dependency.Id));
		}
		connect(view, &QTreeWidget::itemActivated, this, [this](QTreeWidgetItem* item) { ActivateSourceDependency(item); });
		auto* activate = view->addAction("Sync or clean selected dependency", this, [this, view]() { ActivateSourceDependency(view->currentItem()); });
		auto* actionButton = new QToolButton(layout.parentWidget());
		actionButton->setObjectName("StatusActionButton");
		actionButton->setProperty("ActionState", "neutral");
		actionButton->setMinimumHeight(LauncherUi::Row::StatusActionHeight);
		actionButton->setDefaultAction(activate);
		const auto updateAction = [this, view, activate, actionButton]()
		{
			const auto* item = view->currentItem();
			const QString id = item != nullptr ? item->data(0, Qt::UserRole).toString() : QString();
			const bool bCanActivate = item != nullptr && !m_sourceDependencyRunIds.contains(id);
			activate->setText(item != nullptr ? item->text(2) + " " + item->text(0) : "Select a dependency");
			activate->setEnabled(bCanActivate);
			actionButton->setAccessibleName(activate->text());
		};
		connect(view, &QTreeWidget::currentItemChanged, this, updateAction);
		connect(view->model(), &QAbstractItemModel::dataChanged, this, updateAction);
		auto* copy = view->addAction(
		    "Copy dependency details",
		    QKeySequence::Copy,
		    view,
		    [view]()
		    {
			    if (const auto* item = view->currentItem())
			    {
				    QGuiApplication::clipboard()->setText(item->text(0) + "\n" + item->text(1) + "\n" + item->text(3));
			    }
		    });
		copy->setShortcutContext(Qt::WidgetWithChildrenShortcut);
		view->setContextMenuPolicy(Qt::ActionsContextMenu);
		m_sourceDependencyViews.push_back(view);
		RefreshSourceDependencyRows();
		view->setCurrentItem(view->topLevelItem(0));
		updateAction();
		layout.addWidget(view);
		layout.addWidget(actionButton, 0, Qt::AlignLeft);
		emit FocusableWidgetCreated(view);
		emit FocusableWidgetCreated(actionButton);
	}

	void LauncherWorkspaceSetup::RefreshSourceDependencyRows()
	{
		const SparkleTools::ToolOutputPaths workspaceOutputs = SparkleTools::ResolveToolOutputPaths(m_workspaceRoot);
		for (const auto& view : m_sourceDependencyViews)
		{
			if (view == nullptr)
			{
				continue;
			}
			for (int row = 0; row < view->topLevelItemCount(); ++row)
			{
				auto* item = view->topLevelItem(row);
				const QString id = item->data(0, Qt::UserRole).toString();
				const SourceDependencyEntry* dependency = FindSourceDependency(id.toStdString());
				if (dependency == nullptr)
				{
					continue;
				}
				const QString runId = m_sourceDependencyRunIds.value(id);
				const bool bActive = !runId.isEmpty();
				const bool bCleaning = bActive && m_cleaningSourceDependencyRunIds.contains(runId);
				const auto status = bActive ? SourceDependencyStatus{} : InspectDependencyStatus(*dependency, workspaceOutputs.DependencyCacheRoot);
				item->setText(1, bActive ? (bCleaning ? "Cleaning" : "Syncing") : status.Text);
				item->setText(2, bActive ? "In progress" : status.bHasCache ? "Clean" : "Sync");
				item->setText(3, bActive ? QString::fromStdString(dependency->Version) + " · " + SourceDependencyPurpose(*dependency) : status.Detail);
				const auto* color = bActive ? LauncherUi::Color::StateRunning : status.State == "ok" ? LauncherUi::Color::StateSuccess : LauncherUi::Color::StateWarning;
				item->setForeground(1, QColor(color));
				item->setData(2, Qt::AccessibleTextRole, item->text(2) + " " + QString::fromStdString(dependency->Label));
				item->setToolTip(3, item->text(3));
				item->setData(0, Qt::AccessibleDescriptionRole, item->text(1) + ". " + item->text(3));
			}
		}
	}

	void LauncherWorkspaceSetup::ActivateSourceDependency(QTreeWidgetItem* item)
	{
		if (item == nullptr)
		{
			return;
		}
		const QString id = item->data(0, Qt::UserRole).toString();
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
