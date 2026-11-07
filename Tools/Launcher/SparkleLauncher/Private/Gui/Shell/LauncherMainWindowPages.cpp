#include "Gui/Shell/LauncherMainWindow.h"
#include "Gui/Pages/Scenes/LauncherSceneCatalog.h"
#include "Gui/Pages/Workspace/LauncherWorkspaceSetup.h"
#include "Gui/Pages/Build/LauncherBuildOptions.h"
#include "Gui/Pages/Cook/LauncherCookOptions.h"
#include "Gui/Pages/Clean/LauncherCleanOptions.h"
#include "Gui/Widgets/LauncherOptionWidgets.h"
#include "Gui/Models/LauncherWorkflowCatalog.h"
#include "Gui/Models/LauncherContentModel.h"

namespace SparkleLauncher
{
	static bool UsesBuildEnvironmentStatus(const QString& operationId)
	{
		return operationId == "workspace.generate-build-files" || operationId == "workspace.sync-code" || operationId == "workspace.build" || operationId == "workspace.build.editor"
		    || operationId == "launcher.build.self" || operationId == "workspace.build.game" || operationId.startsWith("cook.");
	}

	void LauncherMainWindow::AddOptionsForOperation(QVBoxLayout& layout, const QString& operationId)
	{
		if (operationId == LauncherScenesOperationId())
		{
			m_sceneCatalog->AddSceneSelection(layout);
			return;
		}

		if (operationId == "levels.run")
		{
			AddNoOptionsMessage(layout, "Choose a scene and use the launch settings in the footer.");
			return;
		}

		if (operationId == "workspace.build")
		{
			BuildLauncherBuildOptions(layout, m_settings, [this](QWidget* widget) { RegisterFocusable(widget); }, [this]() { UpdateRunAvailability(); });
			m_workspaceSetup->AddBuildEnvironmentStatus(layout, "workspace.build");
			return;
		}

		if (operationId == "cook.workspace")
		{
			BuildLauncherCookOptions(layout, m_settings, [this](QWidget* widget) { RegisterFocusable(widget); }, [this]() { UpdateRunAvailability(); });
			m_workspaceSetup->AddBuildEnvironmentStatus(layout, "cook.workspace");
			return;
		}

		if (UsesBuildEnvironmentStatus(operationId))
		{
			m_workspaceSetup->AddBuildEnvironmentStatus(layout, operationId);
			return;
		}

		if (operationId == "workspace.clean")
		{
			BuildLauncherCleanOptions(layout, m_workspaceRoot, m_contentModel.ContentId(), m_settings, [this](QWidget* widget) { RegisterFocusable(widget); }, [this]() { UpdateRunAvailability(); });
			return;
		}

		AddNoOptionsMessage(layout, "No settings");
	}
}
