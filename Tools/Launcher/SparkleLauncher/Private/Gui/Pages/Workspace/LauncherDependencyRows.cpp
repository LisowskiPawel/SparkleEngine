#include "Gui/Pages/Workspace/LauncherWorkspaceSetup.h"
#include "Gui/Widgets/LauncherOptionWidgets.h"
#include "Gui/Operations/LauncherOperationConfirmation.h"
#include <utility>

#include "Gui/Style/LauncherUiDesign.h"
#include "Gui/Models/LauncherContentModel.h"
#include "Gui/Models/LauncherDependencyUiModel.h"
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

#include <algorithm>
#include <filesystem>
#include <string>
#include <system_error>

namespace SparkleLauncher
{
	void LauncherWorkspaceSetup::AddSyncDependencies(QVBoxLayout& layout, bool bOptional)
	{
		auto* view = new QTreeWidget(layout.parentWidget());
		view->setObjectName("SourceDependencyView");
		view->setAccessibleName(bOptional ? "Optional source dependencies" : "Source dependencies");
		view->setAccessibleDescription("Select a dependency and use its action button or press Enter to sync or clean. Ctrl+C copies its details.");
		view->setHeaderLabels({"Dependency", "Status", "Action", "Details"});
		view->setRootIsDecorated(false);
		view->setSelectionMode(QAbstractItemView::SingleSelection);
		view->setEditTriggers(QAbstractItemView::NoEditTriggers);
		view->setMinimumHeight(180);
		view->setColumnWidth(0, 190);
		view->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
		view->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
		for (const ThirdPartyDependencyUiEntry& dependency : GetTrackedThirdPartyDependencies())
		{
			if (dependency.bRequired == bOptional)
			{
				continue;
			}

			auto* item = new QTreeWidgetItem(view, {dependency.Label});
			item->setData(0, Qt::UserRole, dependency.Id);
		}
		connect(view, &QTreeWidget::itemActivated, this, [this](QTreeWidgetItem* item) { ActivateSourceDependency(item); });
		auto* activate = view->addAction("Sync or clean selected dependency", view, [this, view]() { ActivateSourceDependency(view->currentItem()); });
		auto* actionButton = new QToolButton(layout.parentWidget());
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
		connect(view, &QTreeWidget::currentItemChanged, view, updateAction);
		connect(view->model(), &QAbstractItemModel::dataChanged, view, updateAction);
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
		const auto& dependencies = GetTrackedThirdPartyDependencies();
		for (const auto& view : m_sourceDependencyViews)
		{
			if (view == nullptr)
			{
				continue;
			}
			for (int row = 0; row < view->topLevelItemCount(); ++row)
			{
				auto* item = view->topLevelItem(row);
				const auto dependency = std::ranges::find(dependencies, item->data(0, Qt::UserRole).toString(), &ThirdPartyDependencyUiEntry::Id);
				if (dependency == dependencies.end())
				{
					continue;
				}
				const QString runId = m_sourceDependencyRunIds.value(dependency->Id);
				const bool bActive = !runId.isEmpty();
				const bool bCleaning = bActive && m_cleaningSourceDependencyRunIds.contains(runId);
				const auto status = bActive ? ThirdPartyDependencyUiStatus{} : BuildThirdPartyDependencyStatus(*dependency, workspaceOutputs.DependencyCacheRoot);
				item->setText(1, bActive ? (bCleaning ? "Cleaning" : "Syncing") : status.Text);
				item->setText(2, bActive ? "In progress" : status.bHasCache ? "Clean" : "Sync");
				item->setText(3, bActive ? dependency->Version + " · " + dependency->Purpose : status.Detail);
				const auto* color = bActive ? LauncherUi::Color::StateRunning : status.State == "ok" ? LauncherUi::Color::StateSuccess : LauncherUi::Color::StateWarning;
				item->setForeground(1, QColor(color));
				item->setData(2, Qt::AccessibleTextRole, item->text(2) + " " + dependency->Label);
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
		const auto& dependencies = GetTrackedThirdPartyDependencies();
		const auto dependency = std::ranges::find(dependencies, id, &ThirdPartyDependencyUiEntry::Id);
		if (dependency == dependencies.end() || m_sourceDependencyRunIds.contains(id))
		{
			return;
		}
		const auto outputs = SparkleTools::ResolveToolOutputPaths(m_workspaceRoot);
		const auto status = BuildThirdPartyDependencyStatus(*dependency, outputs.DependencyCacheRoot);
		if (status.bHasCache)
		{
			CleanSourceDependency(*dependency);
		}
		else
		{
			SyncSourceDependency(*dependency);
		}
	}

	void LauncherWorkspaceSetup::SyncSourceDependency(const ThirdPartyDependencyUiEntry& dependency)
	{
		LauncherOperationRequest request = BuildLauncherOperationRequest(m_workspaceRoot, m_contentModel, m_settings, "workspace.sync-code");
		std::get<BuildWorkspaceOperationRequest>(request.Inputs).SourceDependencyId = dependency.Id.toStdString();
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
		auto& inputs = std::get<MaintenanceOperationRequest>(request.Inputs);
		const SparkleTools::ToolOutputPaths workspaceOutputs = SparkleTools::ResolveToolOutputPaths(m_workspaceRoot);
		const std::filesystem::path dependencyCachePath = workspaceOutputs.DependencyCacheRoot;
		for (const std::filesystem::path& path : GetSourceDependencyCachePaths(*sourceDependency, dependencyCachePath))
		{
			std::error_code errorCode;
			if (!std::filesystem::exists(path, errorCode) || errorCode)
			{
				continue;
			}

			inputs.RequestedCleanTargets.push_back({dependency.Label.toStdString() + " cache", path, "Generated source dependency cache."});
		}
		if (inputs.RequestedCleanTargets.empty())
		{
			return;
		}
		if (!ConfirmLauncherRunRequest(m_dialogParent, request))
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

		const auto& inputs = std::get<BuildWorkspaceOperationRequest>(request.Inputs);
		if (!inputs.SourceDependencyId.empty())
		{
			m_sourceDependencyRunIds.insert(QString::fromStdString(inputs.SourceDependencyId), runId);
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
