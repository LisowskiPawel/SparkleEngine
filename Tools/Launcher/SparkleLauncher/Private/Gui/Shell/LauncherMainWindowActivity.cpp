#include "Gui/Pages/Scenes/LauncherSceneCatalog.h"
#include "Gui/Pages/Workspace/LauncherWorkspaceSetup.h"
#include "GpuCrash/LauncherGpuCrashPanel.h"
#include "Gui/Shell/LauncherMainWindow.h"

#include "Gui/Shell/LauncherActivityPanel.h"
#include "Gui/Models/LauncherOperationResult.h"
#include "Gui/App/LauncherSettings.h"
#include "Gui/Shell/LauncherWorkflowPanel.h"

namespace SparkleLauncher
{
	void LauncherMainWindow::DisplayOperationStarted(const QString& runId, const QString&, const QString& title)
	{
		m_activityPanel->DisplayOperationStarted(runId, title);
		m_workflowPanel->setEnabled(false);
	}

	void LauncherMainWindow::AppendOperationOutput(const QString& runId, const QString&, const QString& outputText)
	{
		m_activityPanel->AppendOperationOutput(runId, outputText);
	}

	void LauncherMainWindow::UpdateOperationProgress(const QString& runId, const QString&, const QString& phase, quint64 completed, quint64 total)
	{
		m_activityPanel->UpdateOperationProgress(runId, phase, completed, total);
	}

	void LauncherMainWindow::DisplayOperationFinished(const QString& runId, const QString& operationId, const QString& title, const LauncherOperationResult& result)
	{
		const bool bSucceeded = result.bSucceeded;
		if (result.GpuCrash)
		{
			m_gpuCrashPanel->SetIncident(*result.GpuCrash);
		}
		if (bSucceeded && !result.ShaderExportPath.isEmpty())
		{
			m_settings.SetShaderExportPath(result.ShaderExportPath);
		}
		m_activityPanel->DisplayOperationFinished(runId, title, result);
		m_workflowPanel->setEnabled(!m_activityPanel->HasActiveRun());

		const bool bSceneStatusHandled = m_sceneCatalog->CompleteOperation(runId, operationId);
		const bool bSetupStatusHandled = m_workspaceSetup->CompleteOperation(runId);
		if (bSceneStatusHandled || bSetupStatusHandled)
		{
			UpdateRunAvailability();
		}
		else
		{
			ScheduleUiRefresh(true);
		}

		const bool bLauncherRestartPending = m_pendingRestartRunIds.removeAll(runId) > 0;
		if (bSucceeded && bLauncherRestartPending)
		{
			PromptForLauncherRestart();
		}
	}
}
