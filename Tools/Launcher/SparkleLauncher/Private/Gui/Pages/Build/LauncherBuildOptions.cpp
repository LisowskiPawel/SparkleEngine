#include "Gui/Pages/Build/LauncherBuildOptions.h"
#include "Gui/Widgets/LauncherOptionWidgets.h"
#include "Gui/Widgets/LauncherSelectionWidgets.h"
#include "Gui/App/LauncherSettings.h"
#include "Gui/Style/LauncherUiDesign.h"
#include "SparkleLauncher/BuildWorkspaceOperations.h"

#include <QtCore/QRegularExpression>
#include <QtCore/QStringList>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace SparkleLauncher
{
	void BuildLauncherBuildOptions(QVBoxLayout& layout, LauncherSettings& settings, const std::function<void(QWidget*)>& registerFocusable, const std::function<void()>& selectionChanged)
	{
		const WorkspaceFeatureSettings features = GetLauncherWorkspaceFeatureSettings();
		const bool bCookToolsAvailable = features.bContentPipelineEnabled || features.bShaderCompilerEnabled;
		const QStringList selectedScopes = settings.BuildScopes().split(QRegularExpression("[,;\\n]"), Qt::SkipEmptyParts);
		QVector<QCheckBox*> scopeBoxes;

		QVBoxLayout* buildLayout = AddOptionGroup(layout, "Choose products", "Select the products to build. CMake resolves their shared dependencies.");

		QFrame* selectionPanel = new QFrame(buildLayout->parentWidget());
		selectionPanel->setObjectName("WorkflowSelectionPanel");
		selectionPanel->setMinimumWidth(LauncherUi::ScopeSelection::ContentMinWidth);
		selectionPanel->setMaximumWidth(LauncherUi::ScopeSelection::ContentMaxWidth);
		QVBoxLayout* selectionLayout = new QVBoxLayout(selectionPanel);
		selectionLayout->setContentsMargins(0, 0, 0, 0);
		selectionLayout->setSpacing(0);
		QVector<QCheckBox*> selectAllBoxes;
		AddWorkflowScopeRow(*selectionLayout, "All", QString(), "Select every available product.", QString(), true, QStringList(), selectAllBoxes, registerFocusable);
		QCheckBox* selectAllBox = selectAllBoxes.front();

		AddWorkflowScopeRow(*selectionLayout, "Editor", "editor", "Authoring, inspection, and developer tools.", settings.EditorProfile(), true, selectedScopes, scopeBoxes, registerFocusable);
		AddWorkflowScopeRow(*selectionLayout, "Game", "game", "Standalone Game product.", settings.GameProfile(), true, selectedScopes, scopeBoxes, registerFocusable);

		AddWorkflowScopeRow(
		    *selectionLayout,
		    "Cooking tools",
		    "cook-tools",
		    bCookToolsAvailable ? "Asset, texture, and shader cooking tools." : "No cooking-tool targets are enabled by this workspace configuration.",
		    bCookToolsAvailable ? settings.EditorProfile() : QStringLiteral("Unavailable"),
		    bCookToolsAvailable,
		    selectedScopes,
		    scopeBoxes,
		    registerFocusable);

		AddWorkflowScopeRow(
		    *selectionLayout,
		    "Launcher",
		    "launcher",
		    "Sparkle Launcher; the new build is used after restart.",
		    settings.EditorProfile(),
		    true,
		    selectedScopes,
		    scopeBoxes,
		    registerFocusable);

		buildLayout->addWidget(selectionPanel);

		AddWorkflowAutomationNote(*buildLayout, "CMake refreshes stale build files and skips current targets.");

		const auto commitSelection = [&settings, scopeBoxes, selectionChanged](bool)
		{
			const QStringList selectedValues = CollectSelectedScopeValues(scopeBoxes);
			settings.SetBuildScopes(selectedValues.join(';'));
			selectionChanged();
		};

		ConnectSelectAllScopeBox(selectAllBox, scopeBoxes, layout.parentWidget(), commitSelection);
		commitSelection(false);
	}
}
