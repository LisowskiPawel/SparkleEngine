#include "Gui/Pages/Clean/LauncherCleanOptions.h"
#include "Gui/Widgets/LauncherOptionWidgets.h"
#include "Gui/Models/LauncherCleanUiModel.h"
#include "Gui/App/LauncherSettings.h"

#include <QtCore/QRegularExpression>
#include <QtCore/QStringList>
#include <QtCore/QSignalBlocker>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItemIterator>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include <array>
#include <system_error>

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

	static QString ToDisplayPath(const std::filesystem::path& workspaceRoot, const std::filesystem::path& path)
	{
		std::error_code errorCode;
		const std::filesystem::path relative = std::filesystem::relative(path, workspaceRoot, errorCode);
		return QString::fromStdString((!errorCode && !relative.empty()) ? relative.generic_string() : path.generic_string());
	}

	static void AddCleanScopeItem(QTreeWidgetItem& parent, const CleanScopeUiOption& scope, const QString& contentId, const QStringList& selectedScopes, const std::filesystem::path& workspaceRoot)
	{
		QString preview = scope.Preview;
		if (scope.Value == "cooked" || preview.isEmpty())
		{
			MaintenanceCleanTarget target{.Path = ResolveCleanScopePreviewPath(workspaceRoot, contentId, scope.Value)};
			InspectMaintenanceCleanTarget(target);
			const QString path = scope.Value == "cooked" ? QStringLiteral("Generated content") : ToDisplayPath(workspaceRoot, target.Path);
			const QString inventory = target.bExists ? QStringLiteral("%1 files, %2 folders").arg(target.FileCount).arg(target.DirectoryCount) : QStringLiteral("not present");
			preview = path + " - " + inventory;
		}
		auto* item = new QTreeWidgetItem(&parent, {scope.Label, scope.Detail, preview});
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
		item->setData(0, Qt::UserRole, scope.Value);
		item->setCheckState(0, selectedScopes.contains("clean-all") || selectedScopes.contains(scope.Value) ? Qt::Checked : Qt::Unchecked);
		item->setToolTip(0, scope.Label);
		item->setToolTip(1, scope.Detail);
		item->setToolTip(2, preview);
		item->setData(0, Qt::AccessibleDescriptionRole, scope.Detail + ". Directory inventory: " + preview);
	}

	void BuildLauncherCleanOptions(
	    QVBoxLayout& layout,
	    const std::filesystem::path& workspaceRoot,
	    const QString& contentId,
	    LauncherSettings& settings,
	    const std::function<void(QWidget*)>& registerFocusable,
	    const std::function<void()>& selectionChanged)
	{
		const std::array<CleanScopeUiOption, 6> cleanScopes{{
		    {"Cooked content", "cooked", "Remove generated cooked assets for this workspace.", QString(), "Content outputs"},
		    {"Build workspace",
		        "build-tree",
		        "Remove build intermediates and generated CMake or IDE files; keep downloaded dependencies.",
		        "build content except build/_deps and generated workspace files",
		        "Build outputs"},
		    {"Generated artifacts", "artifacts", "Remove executables, libraries, symbols, diagnostics, and all cooked content.", QString(), "Build outputs"},
		    {"IDE and workspace state",
		        "workspace-state",
		        "Reset local Visual Studio, Rider, VS Code, and ImGui workspace state.",
		        ".vs, .vscode, .idea, and ImGui workspace state",
		        "Local state and caches"},
		    {"Source dependency cache", "deps", "Remove downloaded code dependencies; the next sync or configure downloads them again.", QString(), "Local state and caches"},
		    {"Logs",
		        "logs",
		        "Remove product and launcher logs from per-user state plus any legacy repository/content logs; keep the current launcher "
		        "log until exit.",
		        "product, launcher, and legacy logs",
		        "Local state and caches"},
		}};

		const std::array<QString, 3> cleanGroups{{
		    "Content outputs",
		    "Build outputs",
		    "Local state and caches",
		}};

		const QStringList selectedScopes = settings.CleanScope().split(QRegularExpression("[,;\\n]"), Qt::SkipEmptyParts);
		QVBoxLayout* cleanLayout = AddOptionGroup(
		    layout,
		    "Choose generated data",
		    "Select only what should be regenerated. Project source files and synced levels are preserved. You will confirm before cleaning. Directory inventories include any preserved files.");

		auto* view = new QTreeWidget(cleanLayout->parentWidget());
		view->setObjectName("CleanScopeView");
		view->setAccessibleName("Generated data to clean");
		view->setAccessibleDescription("Use arrows to select a scope and Space to change its check state. All also includes product user state and captures. Cleaning requires confirmation.");
		view->setHeaderLabels({"Scope", "Description", "Directory inventory"});
		view->setEditTriggers(QAbstractItemView::NoEditTriggers);
		view->setRootIsDecorated(false);
		view->setWordWrap(true);
		view->setMinimumHeight(380);
		view->setColumnWidth(0, 200);
		view->setColumnWidth(1, 400);
		auto* all = new QTreeWidgetItem(
		    view,
		    {"All", "Remove build/cooked outputs, dependency caches and logs.\nIncludes product user state, settings and captures.\nConfirmation is required.", "All generated data"});
		all->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
		all->setCheckState(0, selectedScopes.contains("clean-all") ? Qt::Checked : Qt::Unchecked);
		all->setToolTip(1, all->text(1));
		for (const QString& groupName : cleanGroups)
		{
			auto* group = new QTreeWidgetItem(view, {groupName});
			group->setFlags(Qt::ItemIsEnabled);
			for (const CleanScopeUiOption& scope : cleanScopes)
			{
				if (scope.Group == groupName)
				{
					AddCleanScopeItem(*group, scope, contentId, selectedScopes, workspaceRoot);
				}
			}
		}
		view->expandAll();
		const auto commitSelection = [view, all, &settings, selectionChanged](QTreeWidgetItem* changed)
		{
			const QSignalBlocker blocker(view);
			const bool bCleanAllRequested = changed == all && all->checkState(0) == Qt::Checked;
			QStringList selected;
			int availableCount = 0;
			for (QTreeWidgetItemIterator iterator(view); *iterator; ++iterator)
			{
				auto* item = *iterator;
				const QString value = item->data(0, Qt::UserRole).toString();
				const bool bAvailable = !item->isDisabled() && !value.isEmpty();
				if (!bAvailable)
				{
					continue;
				}
				++availableCount;
				if (changed == all)
				{
					item->setCheckState(0, all->checkState(0));
				}
				if (item->checkState(0) == Qt::Checked)
				{
					selected.push_back(value);
				}
			}
			all->setCheckState(0, availableCount > 0 && selected.size() == availableCount ? Qt::Checked : Qt::Unchecked);
			settings.SetCleanScope(bCleanAllRequested ? QStringLiteral("clean-all") : selected.join(';'));
			selectionChanged();
		};
		QObject::connect(view, &QTreeWidget::itemChanged, view, [commitSelection](QTreeWidgetItem* item, int) { commitSelection(item); });
		cleanLayout->addWidget(view);
		registerFocusable(view);
		commitSelection(selectedScopes.contains("clean-all") ? all : nullptr);
	}
}
