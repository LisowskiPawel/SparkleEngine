#include "Gui/Models/LauncherCleanUiModel.h"
#include "Gui/Pages/Scenes/LauncherSceneCatalog.h"
#include "Gui/Pages/Workspace/LauncherWorkspaceSetup.h"
#include "Gui/Widgets/LauncherOptionWidgets.h"
#include "Gui/Operations/LauncherOperationConfirmation.h"
#include "Gui/Shell/LauncherMainWindow.h"

#include "Gui/Shell/LauncherActivityPanel.h"
#include "Gui/Widgets/LauncherActionWidgets.h"
#include "Gui/App/LauncherBackend.h"
#include "Gui/Models/LauncherOperationRequestFactory.h"
#include "Gui/Operations/LauncherOperationRequestMapping.h"
#include "Gui/Models/LauncherContentModel.h"
#include "Gui/App/LauncherSettings.h"
#include "Gui/Models/LauncherWorkflowCatalog.h"

#include "ToolOutputPaths.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>
#include <QtCore/QRegularExpression>
#include <QtWidgets/QMessageBox>

#include <filesystem>
#include <utility>

namespace SparkleLauncher
{
	void LauncherMainWindow::RunSelectedOperation()
	{
		if (m_selectedOperationId.isEmpty())
		{
			m_activityPanel->ShowMessage("Choose a workflow before running.");
			return;
		}

		if (m_selectedOperationId == LauncherScenesOperationId())
		{
			m_sceneCatalog->SyncAllLevels();
			return;
		}

		if (OperationNeedsContent(m_selectedOperationId) && m_contentModel.ContentId().isEmpty())
		{
			const QString message = "Workspace content is unavailable. Confirm this is a complete Sparkle workspace.";
			m_activityPanel->ShowMessage(message);
			return;
		}
		const QString cleanScopeError = m_selectedOperationId == "workspace.clean" ? CleanScopeSelectionError(m_settings.CleanScope()) : QString();
		if (!cleanScopeError.isEmpty())
		{
			m_activityPanel->ShowMessage(cleanScopeError);
			return;
		}

		if (m_selectedOperationId == "levels.sync")
		{
			m_sceneCatalog->SyncAllLevels();
			return;
		}

		LauncherOperationRequest request = m_selectedOperationId == "workspace.clean"
		    ? BuildScopedCleanOperationRequest(m_workspaceRoot, m_contentModel, m_settings, m_settings.CleanScope(), std::filesystem::path(QCoreApplication::applicationFilePath().toStdString()))
		    : BuildLauncherOperationRequest(m_workspaceRoot, m_contentModel, m_settings, m_selectedOperationId);

		if (!ConfirmLauncherRunRequest(*this, request))
		{
			return;
		}

		const QString title = DisplayNameForOperation(m_selectedOperationId);
		StartOperation(std::move(request), title);
	}

	void LauncherMainWindow::CleanSelectedOperation()
	{
		if (m_selectedOperationId.isEmpty())
		{
			return;
		}
		if (m_selectedOperationId == LauncherScenesOperationId())
		{
			m_sceneCatalog->CleanAllLevels();
			return;
		}

		if (!SupportsActionSpecificClean(m_selectedOperationId.toStdString()))
		{
			return;
		}

		LauncherOperationRequest request = BuildActionCleanOperationRequest(
		    m_workspaceRoot,
		    m_contentModel,
		    m_settings,
		    std::filesystem::path(QCoreApplication::applicationFilePath().toStdString()),
		    m_selectedOperationId);

		if (request.CleanTargets.isEmpty())
		{
			const QString message = OperationNeedsContent(m_selectedOperationId) && m_contentModel.ContentId().isEmpty() ? "Workspace content is unavailable for this workflow's generated outputs."
			                                                                                                             : "No generated outputs were resolved for this workflow.";

			m_activityPanel->ShowMessage(message);
			return;
		}

		if (!ConfirmLauncherRunRequest(*this, request))
		{
			return;
		}

		StartOperation(std::move(request), "Clean " + DisplayNameForOperation(m_selectedOperationId));
	}

	const LauncherOperationDescriptor* LauncherMainWindow::FindOperationDescriptor(const QString& operationId) const
	{
		for (const LauncherOperationDescriptor& operation : m_backend.Operations())
		{
			if (operation.Id == operationId)
			{
				return &operation;
			}
		}
		return nullptr;
	}

	QString LauncherMainWindow::DisplayNameForOperation(const QString& operationId) const
	{
		QString overrideName = LauncherOperationDisplayNameOverride(operationId);
		if (!overrideName.isEmpty())
		{
			return overrideName;
		}
		const LauncherOperationDescriptor* operation = FindOperationDescriptor(operationId);
		return operation == nullptr ? operationId : operation->DisplayName;
	}

	bool LauncherMainWindow::OperationNeedsContent(const QString& operationId) const
	{
		if (operationId == "workspace.clean")
		{
			return CleanScopeSelectionRequiresContent(m_settings.CleanScope());
		}

		if (operationId == "workspace.build")
		{
			return m_settings.BuildScopes().contains("editor") || m_settings.BuildScopes().contains("game");
		}

		return operationId == LauncherScenesOperationId() || operationId == "levels.sync" || operationId == "levels.run" || operationId.startsWith("workspace.build.")
		    || operationId.startsWith("cook.");
	}

	void LauncherMainWindow::PromptForLauncherRestart()
	{
		const QMessageBox::StandardButton result = QMessageBox::question(
		    this,
		    "Launcher Rebuilt",
		    "Sparkle Launcher was rebuilt successfully. Restart now to run the new binary?",
		    QMessageBox::Yes | QMessageBox::No,
		    QMessageBox::Yes);

		if (result != QMessageBox::Yes)
		{
			return;
		}

		const SparkleTools::ToolOutputPaths workspaceOutputs = SparkleTools::ResolveToolOutputPaths(m_workspaceRoot);
		const SparkleTools::ToolTargetOutputPaths launcherOutputs = workspaceOutputs.LauncherTargetOutputs(m_settings.EditorProfile().toStdString());
		const std::filesystem::path relaunchedExecutablePath = launcherOutputs.BinaryDirectory / std::filesystem::path(QCoreApplication::applicationFilePath().toStdString()).filename();
		const QString executablePath = QString::fromStdString(relaunchedExecutablePath.string());
		const bool bStarted = QProcess::startDetached(executablePath, {});
		if (!bStarted)
		{
			QMessageBox::warning(this, "Restart Failed", "The rebuilt launcher is ready, but the restart command could not be started.");
			return;
		}

		QCoreApplication::quit();
	}

	QString LauncherMainWindow::CreateRunId()
	{
		return QStringLiteral("run-%1").arg(++m_nextRunIndex, 4, 10, QChar('0'));
	}

	QString LauncherMainWindow::StartOperation(LauncherOperationRequest request, const QString& title)
	{
		if (request.RunId.isEmpty())
		{
			request.RunId = CreateRunId();
		}
		const QString runId = request.RunId;
		if (LauncherOperationRequestMapping::RequestsLauncherRebuild(request) && !m_pendingRestartRunIds.contains(runId))
		{
			m_pendingRestartRunIds.push_back(runId);
		}
		m_activityPanel->RegisterRun(request.RunId, title);
		m_workspaceSetup->TrackSourceDependencyRun(request, runId);
		m_backend.RunOperation(std::move(request));
		return runId;
	}
}
