#include "Gui/Pages/Clean/LauncherCleanOptions.h"
#include "Gui/Widgets/LauncherOptionWidgets.h"
#include "Gui/Widgets/LauncherSelectionWidgets.h"
#include "Gui/Models/LauncherCleanUiModel.h"
#include "Gui/App/LauncherSettings.h"
#include "Gui/Style/LauncherUiDesign.h"

#include <QtCore/QRegularExpression>
#include <QtCore/QStringList>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
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

	static void AddCleanScopeRow(
	    QVBoxLayout& layout,
	    const CleanScopeUiOption& scope,
	    const QString& contentId,
	    const QStringList& selectedScopes,
	    QVector<QCheckBox*>& scopeBoxes,
	    const std::filesystem::path& workspaceRoot,
	    const std::function<void(QWidget*)>& registerFocusable)
	{
		QString preview = scope.Preview;
		QString inventory;
		if (scope.Value == "cooked" || preview.isEmpty())
		{
			MaintenanceCleanTarget target{.Path = ResolveCleanScopePreviewPath(workspaceRoot, contentId, scope.Value)};
			InspectMaintenanceCleanTarget(target);
			const QString path = scope.Value == "cooked" ? QStringLiteral("Generated content") : ToDisplayPath(workspaceRoot, target.Path);
			inventory = target.bExists ? QStringLiteral("%1 files, %2 folders").arg(target.FileCount).arg(target.DirectoryCount) : QStringLiteral("Not present");
			preview = path + " - " + inventory;
		}

		AddWorkflowScopeRow(layout, scope.Label, scope.Value, scope.Detail, inventory, true, selectedScopes, scopeBoxes, registerFocusable);
		QCheckBox* scopeBox = scopeBoxes.back();
		const QString tooltip = scope.Detail + "\n" + preview + "\nDirectory inventories include any preserved files.";
		scopeBox->setToolTip(tooltip);
		scopeBox->setAccessibleDescription(tooltip);
		scopeBox->parentWidget()->setToolTip(tooltip);
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
		    {"Cooked content", "cooked", "Generated assets; cook again to restore.", QString(), "Content outputs"},
		    {"Build workspace", "build-tree", "Build intermediates and IDE files; keeps downloaded dependencies.", "build content except build/_deps and generated workspace files", "Build outputs"},
		    {"Generated artifacts", "artifacts", "Binaries, symbols, diagnostics and all cooked content.", QString(), "Build outputs"},
		    {"IDE and workspace state", "workspace-state", "Reset local IDE and ImGui settings.", ".vs, .vscode, .idea, and ImGui workspace state", "Local state and caches"},
		    {"Source dependency cache", "deps", "Downloaded code; sync again to restore.", QString(), "Local state and caches"},
		    {"Logs", "logs", "Product and Launcher logs; keeps the current log until exit.", "product, launcher, and legacy logs", "Local state and caches"},
		}};

		const std::array<QString, 3> cleanGroups{{
		    "Content outputs",
		    "Build outputs",
		    "Local state and caches",
		}};

		QStringList selectedScopes = settings.CleanScope().split(QRegularExpression("[,;\\n]"), Qt::SkipEmptyParts);
		const bool bCleanAllRequested = selectedScopes.contains("clean-all");
		if (bCleanAllRequested)
		{
			for (const CleanScopeUiOption& scope : cleanScopes)
			{
				selectedScopes.push_back(scope.Value);
			}
		}

		QVBoxLayout* cleanLayout = AddOptionGroup(layout, "Choose generated data", "Select what to regenerate. Source files and synced levels are kept. Cleaning requires confirmation.");

		auto* selectionPanel = new QFrame(cleanLayout->parentWidget());
		selectionPanel->setObjectName("WorkflowSelectionPanel");
		selectionPanel->setAccessibleName("Generated data to clean");
		selectionPanel->setMinimumWidth(LauncherUi::ScopeSelection::ContentMinWidth);
		selectionPanel->setMaximumWidth(LauncherUi::ScopeSelection::ContentMaxWidth);
		auto* selectionLayout = new QVBoxLayout(selectionPanel);
		selectionLayout->setContentsMargins(0, 0, 0, 0);
		selectionLayout->setSpacing(0);
		QVector<QCheckBox*> selectAllBoxes;
		AddWorkflowScopeRow(*selectionLayout, "All", "clean-all", "All generated data, including product settings and captures.", QString(), true, selectedScopes, selectAllBoxes, registerFocusable);

		QCheckBox* selectAllBox = selectAllBoxes.front();
		QVector<QCheckBox*> scopeBoxes;
		for (const QString& groupName : cleanGroups)
		{
			selectionLayout->addWidget(CreateSectionLabel(groupName));
			for (const CleanScopeUiOption& scope : cleanScopes)
			{
				if (scope.Group == groupName)
				{
					AddCleanScopeRow(*selectionLayout, scope, contentId, selectedScopes, scopeBoxes, workspaceRoot, registerFocusable);
				}
			}
		}
		cleanLayout->addWidget(selectionPanel);
		const auto commitSelection = [&settings, scopeBoxes, selectionChanged](bool bCleanAll)
		{
			const QStringList selected = CollectSelectedScopeValues(scopeBoxes);
			settings.SetCleanScope(bCleanAll ? QStringLiteral("clean-all") : selected.join(';'));
			selectionChanged();
		};

		ConnectSelectAllScopeBox(selectAllBox, scopeBoxes, layout.parentWidget(), commitSelection);
		commitSelection(bCleanAllRequested);
	}
}
