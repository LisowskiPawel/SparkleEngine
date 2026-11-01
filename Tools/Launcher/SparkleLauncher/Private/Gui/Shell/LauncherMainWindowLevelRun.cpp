#include "LauncherMainWindow.h"

#include "LauncherActivityPanel.h"
#include "LauncherContentModel.h"
#include "LauncherLevelUiModel.h"
#include "LauncherOperationRequestFactory.h"
#include "LauncherSettings.h"

#include <utility>

namespace SparkleLauncher
{
	void LauncherMainWindow::StartLevel(const LauncherContentSummary& content, const LauncherLevelUiEntry& level)
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
		m_startingLevelRunId = StartOperation(std::move(request), action + level.DisplayName);
		RefreshLevelActionButtons();
	}
}
