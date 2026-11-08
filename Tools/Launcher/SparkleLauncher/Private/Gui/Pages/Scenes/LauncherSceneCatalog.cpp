#include "Gui/Pages/Scenes/LauncherSceneCatalog.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Gui/Widgets/LauncherOptionWidgets.h"
#include "Gui/Operations/LauncherOperationConfirmation.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>

#include "Gui/Widgets/LauncherActionWidgets.h"
#include "Gui/Shell/LauncherActivityPanel.h"
#include "Gui/Models/LauncherLevelUiModel.h"
#include "Gui/Models/LauncherOperationRequestFactory.h"
#include "Gui/App/LauncherSettings.h"
#include "Gui/Models/LauncherContentModel.h"

#include "SparkleLauncher/MaintenanceOperations.h"
#include <QtCore/QStringList>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include <exception>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace SparkleLauncher
{
	LauncherSceneCatalog::LauncherSceneCatalog(
	    const std::filesystem::path& workspaceRoot,
	    LauncherContentModel& contentModel,
	    LauncherSettings& settings,
	    LauncherActivityPanel& activityPanel,
	    std::function<QString(LauncherOperationRequest, const QString&)> startOperation,
	    QWidget& dialogParent) :
	    QObject(&dialogParent),
	    m_workspaceRoot(workspaceRoot),
	    m_contentModel(contentModel),
	    m_settings(settings),
	    m_activityPanel(&activityPanel),
	    m_startOperation(std::move(startOperation)),
	    m_dialogParent(dialogParent)
	{
	}

	void LauncherSceneCatalog::ResetWidgets()
	{
		m_levelStatusLabels.clear();
		m_levelActionButtons.clear();
	}

	bool LauncherSceneCatalog::CompleteOperation(const QString& runId, const QString& operationId)
	{
		const bool bCatalogChanged = operationId == "levels.sync" || operationId == "workspace.clean";
		const bool bLaunchSettled = runId == m_startingLevelRunId;
		if (bLaunchSettled)
		{
			m_startingLevelRunId.clear();
			m_startingLevelId.clear();
		}
		if (bCatalogChanged || bLaunchSettled)
		{
			RefreshLevelActionButtons();
		}
		return bCatalogChanged;
	}

	void LauncherSceneCatalog::AddLevelContentGroups(QVBoxLayout& layout)
	{
		QVBoxLayout* catalogLayout = AddOptionGroup(layout, "Levels", QString());
		const LauncherContentSummary* content = m_contentModel.Content();
		if (content == nullptr)
		{
			AddNoOptionsMessage(*catalogLayout, "Workspace content is unavailable.");
			return;
		}

		const LauncherLevelUiModel model = LauncherLevelUiModel::Build(*content);
		if (!model.bLoaded)
		{
			AddNoOptionsMessage(*catalogLayout, model.LoadError.isEmpty() ? QStringLiteral("The repository has no readable Levels.catalog.") : model.LoadError);
			return;
		}

		AddSyncLevelRows(*catalogLayout, *content, model);
	}

	void LauncherSceneCatalog::RefreshLevelActionButtons()
	{
		const LauncherContentSummary* content = m_contentModel.Content();
		if (content == nullptr)
		{
			return;
		}

		const LauncherLevelUiModel model = LauncherLevelUiModel::Build(*content);
		if (!model.bLoaded)
		{
			return;
		}

		for (const LauncherLevelUiEntry& level : model.Levels)
		{
			QLabel* statusLabel = m_levelStatusLabels.value(level.Id, nullptr);
			QPushButton* button = m_levelActionButtons.value(level.Id, nullptr);
			if (statusLabel != nullptr && button != nullptr)
			{
				ApplyLevelActionButtonState(*statusLabel, *button, level);
			}
		}
	}

	void LauncherSceneCatalog::SyncAllLevels()
	{
		LauncherOperationRequest request = BuildLauncherOperationRequest(m_workspaceRoot, m_contentModel, m_settings, QStringLiteral("levels.sync"));
		request.bSelectAvailableLevels = true;
		m_startOperation(std::move(request), QStringLiteral("Sync Available Levels"));
	}

	void LauncherSceneCatalog::CleanAllLevels()
	{
		LauncherOperationRequest request = BuildLauncherOperationRequest(m_workspaceRoot, m_contentModel, m_settings, QStringLiteral("workspace.clean"));
		request.bCleanLevelContent = true;
		request.bConfirmClean = false;
		if (ConfirmLauncherRunRequest(m_dialogParent, request))
		{
			m_startOperation(std::move(request), QStringLiteral("Clean All Levels"));
		}
	}

	LauncherLevelUiModel LauncherSceneCatalog::BuildLevelUiModel() const
	{
		const LauncherContentSummary* content = m_contentModel.Content();
		if (content == nullptr)
		{
			return {};
		}

		return LauncherLevelUiModel::Build(*content);
	}

	void LauncherSceneCatalog::UpdateActionAvailability(QPushButton& cleanButton, QPushButton& runButton, bool bHasActionCleanTargets)
	{
		bool bCanSyncLevel = false;
		bool bHasSelectedLevels = false;
		bool bHasExtractedLevelContent = false;
		const LauncherLevelUiModel levelModel = BuildLevelUiModel();
		for (const LauncherLevelUiEntry& level : levelModel.Levels)
		{
			const bool bSupportedLevel = level.Id != "Empty" && level.bGameSupported;
			const bool bSelectableLevel = bSupportedLevel && level.bCanPrepare;
			bCanSyncLevel = bCanSyncLevel || bSelectableLevel;
			if (level.Id != "Empty" && level.bSelected)
			{
				bHasSelectedLevels = true;
				break;
			}
		}

		const LauncherContentSummary* content = m_contentModel.Content();
		if (content != nullptr)
		{
			try
			{
				bHasExtractedLevelContent = !BuildLevelCleanPaths(content->RootPath, {}).empty();
			}
			catch (const Diagnostics::Error&)
			{
				// The catalog page owns the detailed load diagnostic. Keep the footer safely disabled here.
			}
		}
		const bool bCanCleanLevelContent = bHasSelectedLevels || bHasExtractedLevelContent;
		const bool bCanClean = bCanCleanLevelContent || (bHasActionCleanTargets);

		ApplyActionAvailability(
		    cleanButton,
		    bCanClean,
		    bCanClean ? "Disable all selected levels and clean extracted external level content. Cached archives are preserved." : "No selected levels are available to clean.");

		const QString reason = bCanSyncLevel ? QStringLiteral("Sync all available levels. Existing runs keep going.") : QStringLiteral("No catalog levels are available to sync.");
		ApplyActionAvailability(runButton, bCanSyncLevel, reason);
	}
}
