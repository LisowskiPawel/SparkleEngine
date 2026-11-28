#include "Gui/Pages/Scenes/LauncherSceneCatalog.h"
#include "Gui/Pages/Workspace/LauncherWorkspaceSetup.h"
#include "Gui/Shell/LauncherMainWindow.h"

#include "Gui/Shell/LauncherActivityPanel.h"
#include "Gui/App/LauncherSettings.h"
#include "Gui/Shell/LauncherWorkflowPanel.h"

namespace SparkleLauncher
{
	void LauncherMainWindow::DisplayOperationStarted(const QString& runId, const QString& title)
	{
		m_activityPanel->DisplayOperationStarted(runId, title);
		m_workflowPanel->setEnabled(false);
	}

	void LauncherMainWindow::AppendOperationOutput(const QString& runId, const QString& outputText)
	{
		m_activityPanel->AppendOperationOutput(runId, outputText);
	}

	void LauncherMainWindow::UpdateOperationProgress(const QString& runId, const QString& phase, quint64 completed, quint64 total)
	{
		m_activityPanel->UpdateOperationProgress(runId, phase, completed, total);
	}

	void LauncherMainWindow::DisplayOperationFinished(const QString& runId, const OperationRecord& result)
	{
		const bool bSucceeded = result.Status == OperationStatus::Succeeded;
		if (bSucceeded && !result.ShaderExportPath.empty())
		{
			m_settings.SetShaderExportPath(QString::fromStdWString(result.ShaderExportPath.wstring()));
		}
		m_activityPanel->DisplayOperationFinished(runId, result);
		m_workflowPanel->setEnabled(!m_activityPanel->HasActiveRun());

		const bool bSceneStatusHandled = m_sceneCatalog->CompleteOperation(runId, QString::fromStdString(result.Id));
		const bool bSetupStatusHandled = m_workspaceSetup->CompleteOperation(runId);
		if (bSceneStatusHandled || bSetupStatusHandled)
		{
			UpdateRunAvailability();
		}
		else
		{
			ScheduleUiRefresh(true);
		}
	}
}
