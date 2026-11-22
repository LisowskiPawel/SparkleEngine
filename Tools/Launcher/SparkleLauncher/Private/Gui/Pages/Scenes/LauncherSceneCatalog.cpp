#include "Gui/Pages/Scenes/LauncherSceneCatalog.h"
#include "Gui/Pages/Scenes/LauncherSceneView.h"
#include "Gui/Widgets/LauncherSceneWidgets.h"
#include "Gui/Style/LauncherUiDesign.h"
#include "Gui/Widgets/LauncherOptionWidgets.h"
#include "Core/Public/Diagnostics/Error.h"
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
#include <QtWidgets/QWidget>

#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QMessageBox>
#include <QtGui/QDesktopServices>
#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>
#include <QtCore/QUrl>
#include <algorithm>
#include <utility>

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

	LauncherSceneCatalog::~LauncherSceneCatalog()
	{
		delete m_view;
	}

	void LauncherSceneCatalog::Refresh()
	{
		m_levelModel.Refresh(m_contentModel.Content(), m_settings, m_startingLevelRunId.isEmpty() ? QString() : m_startingLevelId);
		if (m_catalogMessage != nullptr)
		{
			const QString message = !m_levelModel.Error().isEmpty() ? m_levelModel.Error() : m_levelModel.rowCount() == 0 ? QStringLiteral("The catalogue has no levels.") : QString();
			m_catalogMessage->setText(message);
			m_catalogMessage->setVisible(!message.isEmpty());
		}
	}

	bool LauncherSceneCatalog::CompleteOperation(const QString& runId, const QString& operationId)
	{
		const bool bCatalogChanged = operationId == "levels.sync" || operationId == "workspace.clean";
		const bool bLaunchSettled = !m_startingLevelRunId.isEmpty() && runId == m_startingLevelRunId;
		if (bLaunchSettled)
		{
			m_startingLevelRunId.clear();
			m_startingLevelId.clear();
		}
		if (bCatalogChanged || bLaunchSettled)
		{
			Refresh();
		}
		return bCatalogChanged || bLaunchSettled;
	}

	void LauncherSceneCatalog::SyncAllLevels()
	{
		LauncherOperationRequest request = BuildLauncherOperationRequest(m_workspaceRoot, m_contentModel, m_settings, QStringLiteral("levels.sync"));
		std::get<LevelOperationRequest>(request.Inputs).bSelectAvailableLevels = true;
		m_startOperation(std::move(request), QStringLiteral("Sync Available Levels"));
	}

	void LauncherSceneCatalog::CleanAllLevels()
	{
		LauncherOperationRequest request = BuildLauncherOperationRequest(m_workspaceRoot, m_contentModel, m_settings, QStringLiteral("workspace.clean"));
		std::get<MaintenanceOperationRequest>(request.Inputs).bCleanLevelContent = true;
		if (ConfirmLauncherRunRequest(m_dialogParent, request))
		{
			m_startOperation(std::move(request), QStringLiteral("Clean All Levels"));
		}
	}

	void LauncherSceneCatalog::UpdateActionAvailability(QPushButton& cleanButton, QPushButton& runButton, bool bHasActionCleanTargets)
	{
		bool bCanSyncLevel = false;
		bool bHasSelectedLevels = false;
		bool bHasExtractedLevelContent = false;
		for (const LauncherLevelUiEntry& level : m_levelModel.Levels())
		{
			const bool bSupportedLevel = level.Id != "Empty" && level.bGameSupported;
			const bool bSelectableLevel = bSupportedLevel && level.bCanPrepare;
			bCanSyncLevel = bCanSyncLevel || bSelectableLevel;
			if (level.Id != "Empty" && level.bSelected)
			{
				bHasSelectedLevels = true;
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

	void LauncherSceneCatalog::AddSceneSelection(QVBoxLayout& layout, QScrollArea& scrollArea)
	{
		const QString heroDetail = QStringLiteral("Choose a scene and open Editor or Game. Sync, Build and Cook are optional developer tools.");
		layout.addWidget(CreateSceneHeroCard(m_workspaceRoot, "Explore Sparkle", heroDetail, "neutral", nullptr, nullptr, "showcase-hero.png", layout.parentWidget()));
		auto* body = new QWidget(layout.parentWidget());
		body->setObjectName("SceneSelectionBody");
		auto* bodyLayout = new QVBoxLayout(body);
		// The view includes a trailing cell gutter; the visible cards retain the original right margin.
		bodyLayout
		    ->setContentsMargins(LauncherUi::Scenes::BodyLeft, LauncherUi::Scenes::BodyTop, LauncherUi::Scenes::BodyRight - LauncherUi::MapCatalog::HorizontalSpacing, LauncherUi::Scenes::BodyBottom);
		bodyLayout->setSpacing(LauncherUi::Scenes::SectionSpacing);
		QVBoxLayout* catalogue = AddOptionGroup(*bodyLayout, "Levels", QString());
		m_catalogMessage = new QLabel(catalogue->parentWidget());
		m_catalogMessage->setObjectName("MutedLabel");
		m_catalogMessage->setWordWrap(true);
		catalogue->addWidget(m_catalogMessage);
		Refresh();
		m_view = new LauncherSceneView(m_levelModel, catalogue->parentWidget(), [this](QWidget* widget) { emit FocusableWidgetCreated(widget); });
		catalogue->addWidget(m_view);
		connect(m_view, &LauncherSceneView::ActionRequested, this, &LauncherSceneCatalog::DispatchAction);
		connect(
		    m_view,
		    &LauncherSceneView::CurrentItemChanged,
		    &scrollArea,
		    [this, &scrollArea](const QRect& rect)
		    {
			    const QPoint center = m_view->viewport()->mapTo(scrollArea.widget(), rect.center());
			    scrollArea.ensureVisible(center.x(), center.y(), rect.width() / 2, rect.height() / 2);
		    });
		layout.addWidget(body);
	}

	void LauncherSceneCatalog::DispatchAction(const QString& contentId, const QString& levelId, LauncherLevelAction action)
	{
		if (contentId != m_contentModel.ContentId())
		{
			return;
		}
		Refresh();
		const auto& levels = m_levelModel.Levels();
		const auto found = std::find_if(levels.begin(), levels.end(), [&levelId](const auto& level) { return level.Id == levelId; });
		const LauncherContentSummary* content = m_contentModel.Content();
		if (found == levels.end() || content == nullptr)
		{
			return;
		}
		// Use the current row only until request construction; no row reference survives dispatch.
		const LauncherLevelUiEntry& level = *found;
		if (action == LauncherLevelAction::Source)
		{
			if (!level.SourcePageUrl.isEmpty())
			{
				QDesktopServices::openUrl(QUrl(level.SourcePageUrl));
			}
		}
		else if (action == LauncherLevelAction::CopyDetails)
		{
			QGuiApplication::clipboard()->setText(level.Detail);
		}
		else if (action == level.Action && action == LauncherLevelAction::Run)
		{
			StartLevel(*content, level);
		}
		else if (action == level.Action && action == LauncherLevelAction::Sync)
		{
			LauncherOperationRequest request = BuildLauncherOperationRequest(m_workspaceRoot, m_contentModel, m_settings, QStringLiteral("levels.sync"));
			std::get<LevelOperationRequest>(request.Inputs).RequestedLevelIds = {level.Id.toStdString()};
			m_startOperation(std::move(request), QStringLiteral("Sync %1").arg(level.DisplayName));
		}
		else if (action == level.Action && action == LauncherLevelAction::Details)
		{
			QMessageBox::information(&m_dialogParent, level.DisplayName, level.ActionToolTip);
		}
	}

	void LauncherSceneCatalog::StartLevel(const LauncherContentSummary& content, const LauncherLevelUiEntry& level)
	{
		if (!m_startingLevelRunId.isEmpty())
		{
			m_activityPanel->ShowRunOutput(m_startingLevelRunId);
			return;
		}
		if (!level.bGameSupported || !level.bCanSelect)
		{
			return;
		}

		LauncherOperationRequest request = BuildLauncherOperationRequest(m_workspaceRoot, m_contentModel, m_settings, QStringLiteral("levels.run"));
		auto& inputs = std::get<LevelRunOperationRequest>(request.Inputs);
		inputs.ContentId = content.Id.toStdString();
		inputs.LevelId = level.Id.toStdString();
		const QString action = m_settings.RunMode() == QStringLiteral("game") ? QStringLiteral("Run ") : QStringLiteral("Open ");
		m_startingLevelId = level.Id;
		m_startingLevelRunId = m_startOperation(std::move(request), action + level.DisplayName);
		if (m_startingLevelRunId.isEmpty())
		{
			m_startingLevelId.clear();
		}
		Refresh();
	}
}
