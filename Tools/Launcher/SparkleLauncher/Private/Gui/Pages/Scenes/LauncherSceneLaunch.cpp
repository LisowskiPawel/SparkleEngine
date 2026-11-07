#include "Gui/Pages/Scenes/LauncherSceneCatalog.h"
#include "Gui/Widgets/LauncherOptionWidgets.h"
#include "Gui/Operations/LauncherOperationConfirmation.h"
#include "Gui/Models/LauncherOperationResult.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>

#include "Gui/Shell/LauncherActivityPanel.h"
#include "Gui/Models/LauncherContentModel.h"
#include "Gui/Models/LauncherLevelUiModel.h"
#include "Gui/Models/LauncherOperationRequestFactory.h"
#include "Gui/App/LauncherSettings.h"

#include <utility>

namespace SparkleLauncher
{
	void LauncherSceneCatalog::StartLevel(const LauncherContentSummary& content, const LauncherLevelUiEntry& level)
	{
		if (!m_startingLevelRunId.isEmpty())
		{
			m_activityPanel->ShowRunOutput(m_startingLevelRunId);
			return;
		}
		if (!level.GameSupported || !level.CanSelect)
		{
			return;
		}

		LauncherOperationRequest request = BuildLauncherOperationRequest(m_workspaceRoot, m_contentModel, m_settings, QStringLiteral("levels.run"));
		request.ContentId = content.Id;
		request.RequestedLevelIds = level.Id;
		const QString action = m_settings.RunMode() == QStringLiteral("game") ? QStringLiteral("Run ") : QStringLiteral("Open ");
		m_startingLevelId = level.Id;
		m_startingLevelRunId = m_startOperation(std::move(request), action + level.DisplayName);
		RefreshLevelActionButtons();
	}
}
