#include "Gui/Pages/Scenes/LauncherSceneCatalog.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Gui/Widgets/LauncherOptionWidgets.h"
#include "Gui/Operations/LauncherOperationConfirmation.h"
#include "Gui/Models/LauncherOperationResult.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>

#include "Gui/Widgets/LauncherActionWidgets.h"
#include "Gui/Shell/LauncherActivityPanel.h"
#include "Gui/Widgets/LauncherArtworkWidgets.h"
#include "Gui/Widgets/LauncherLayoutWidgets.h"
#include "Gui/Models/LauncherLevelUiModel.h"
#include "Gui/Models/LauncherOperationRequestFactory.h"
#include "Gui/App/LauncherSettings.h"
#include "Gui/Models/LauncherContentModel.h"
#include "Gui/Style/LauncherUiDesign.h"

#include "ProjectLevelCatalog.h"
#include "SparkleLauncher/MaintenanceOperations.h"
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtGui/QDesktopServices>
#include <QtGui/QLinearGradient>
#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtGui/QPixmap>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QSizePolicy>
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

	bool LauncherSceneCatalog::CompleteOperation(const QString& runId, const QString& operationId, const QString& title, const LauncherOperationResult& result)
	{
		const bool bCatalogChanged = operationId == QStringLiteral("levels.sync") || m_pendingLevelSelectionUpdates.contains(runId);
		if (m_pendingLevelSelectionUpdates.contains(runId))
		{
			const PendingLevelSelectionUpdate update = m_pendingLevelSelectionUpdates.take(runId);
			if (result.bSucceeded)
			{
				SetLevelsSelected(update.ContentRoot, update.LevelIds, update.bSelected, title);
			}
		}
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

	static QPixmap CreateMapPlaceholder(const LauncherLevelUiEntry& level)
	{
		QPixmap placeholder(640, 400);
		if (level.Id == "Empty")
		{
			QPainter painter(&placeholder);
			painter.fillRect(placeholder.rect(), QColor("#101310"));
			painter.setPen(QPen(QColor(118, 185, 45, 45), 1));
			const QPoint horizon(placeholder.width() / 2, placeholder.height() * 2 / 5);
			for (int x = -160; x <= placeholder.width() + 160; x += 64)
			{
				painter.drawLine(horizon, QPoint(x, placeholder.height()));
			}
			for (int y = horizon.y(); y <= placeholder.height(); y += 32)
			{
				painter.drawLine(0, y, placeholder.width(), y);
			}

			painter.setPen(QPen(QColor("#76b92d"), 4));
			const QRect front(placeholder.width() / 2 - 58, placeholder.height() / 2 - 34, 116, 92);
			const QPoint offset(38, -28);
			const QRect back = front.translated(offset);
			painter.drawRect(front);
			painter.drawRect(back);
			painter.drawLine(front.topLeft(), back.topLeft());
			painter.drawLine(front.topRight(), back.topRight());
			painter.drawLine(front.bottomLeft(), back.bottomLeft());
			painter.drawLine(front.bottomRight(), back.bottomRight());
			return placeholder;
		}

		const uint hueSeed = qHash(level.Id);
		const QColor topColor = QColor::fromHsv(static_cast<int>(hueSeed % 80u) + 75, 125, 74);
		const QColor bottomColor = QColor::fromHsv(static_cast<int>((hueSeed + 22u) % 80u) + 75, 175, 24);
		QLinearGradient gradient(0, 0, placeholder.width(), placeholder.height());
		gradient.setColorAt(0.0, topColor);
		gradient.setColorAt(1.0, bottomColor);

		QPainter painter(&placeholder);
		painter.fillRect(placeholder.rect(), gradient);
		painter.setPen(QPen(QColor(255, 255, 255, 28), 3));
		painter.drawLine(0, placeholder.height() * 3 / 4, placeholder.width(), placeholder.height() / 3);
		painter.drawEllipse(QPoint(placeholder.width() * 4 / 5, placeholder.height() / 5), 84, 84);
		painter.setPen(QPen(QColor(232, 242, 224, 115), 5));
		const QRect marker(placeholder.width() / 2 - 54, placeholder.height() / 2 - 44, 108, 88);
		painter.drawRoundedRect(marker, 8, 8);
		painter.drawLine(marker.left() + 18, marker.bottom() - 18, marker.center().x(), marker.top() + 20);
		painter.drawLine(marker.center().x(), marker.top() + 20, marker.right() - 18, marker.bottom() - 18);
		return placeholder;
	}

	static QPixmap LoadMapArtwork(const LauncherLevelUiEntry& level)
	{
		if (!level.ThumbnailPath.isEmpty() && QFileInfo::exists(level.ThumbnailPath))
		{
			QPixmap artwork(level.ThumbnailPath);
			if (!artwork.isNull())
			{
				return artwork;
			}
		}
		return CreateMapPlaceholder(level);
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

	void LauncherSceneCatalog::AddSyncLevelRows(QVBoxLayout& layout, const LauncherContentSummary& content, const LauncherLevelUiModel& model)
	{
		ResponsiveCardGridWidget* grid = new ResponsiveCardGridWidget(
		    LauncherUi::MapCatalog::MinimumCardWidth,
		    LauncherUi::MapCatalog::MaximumCardWidth,
		    LauncherUi::MapCatalog::MaximumColumns,
		    LauncherUi::MapCatalog::HorizontalSpacing,
		    LauncherUi::MapCatalog::VerticalSpacing,
		    layout.parentWidget());

		for (const LauncherLevelUiEntry& level : model.Levels)
		{
			AddSyncLevelRow(*grid, content, level);
		}
		layout.addWidget(grid);
	}

	void LauncherSceneCatalog::AddSyncLevelRow(ResponsiveCardGridWidget& grid, const LauncherContentSummary& content, const LauncherLevelUiEntry& level)
	{
		ProportionalCardFrame* card = new ProportionalCardFrame(LauncherUi::MapCatalog::CardAspectRatio, &m_dialogParent);
		card->setObjectName("MapCatalogCard");
		card->setProperty("State", level.State);
		card->setAccessibleName(level.DisplayName);
		card->setAccessibleDescription(level.Description);

		QHBoxLayout* cardLayout = new QHBoxLayout(card);
		cardLayout->setContentsMargins(0, 0, 0, 0);
		cardLayout->setSpacing(0);
		LauncherArtworkSpec artworkSpec;
		artworkSpec.BaseColor = QColor("#0b0d0c");
		artworkSpec.AspectRatio = 16.0 / 10.0;
		LauncherArtworkWidget* artwork = new LauncherArtworkWidget(LoadMapArtwork(level), artworkSpec, QSize(640, 400), card);
		artwork->setObjectName("MapCardThumbnail");
		artwork->setMinimumWidth(160);
		cardLayout->addWidget(artwork, 2);

		QWidget* body = new QWidget(card);
		body->setObjectName("MapCardBody");
		body->setAttribute(Qt::WA_StyledBackground, true);
		QVBoxLayout* bodyLayout = new QVBoxLayout(body);
		bodyLayout->setContentsMargins(LauncherUi::MapCatalog::CardBodyMargins);
		bodyLayout->setSpacing(LauncherUi::MapCatalog::CardContentSpacing);

		ElidedLabel* title = new ElidedLabel(level.DisplayName, body);
		title->setObjectName("MapCardTitle");
		title->setProperty("State", level.State);
		title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		bodyLayout->addWidget(title);

		QLabel* description = new QLabel(level.Description.isEmpty() ? QStringLiteral("Level") : level.Description, body);
		description->setObjectName("MapCardDescription");
		description->setWordWrap(true);
		description->setAlignment(Qt::AlignLeft | Qt::AlignTop);
		bodyLayout->addWidget(description, 1);

		QHBoxLayout* metadata = new QHBoxLayout();
		metadata->setContentsMargins(2, 0, 2, 0);
		metadata->setSpacing(8);
		QLabel* statusLabel = new QLabel(body);
		statusLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		metadata->addWidget(statusLabel, 0, Qt::AlignVCenter);

		ElidedLabel* detail = new ElidedLabel(level.Detail, body);
		detail->setObjectName("MapCardMeta");
		detail->setTextInteractionFlags(Qt::TextSelectableByMouse);
		metadata->addWidget(detail, 1, Qt::AlignVCenter);
		bodyLayout->addLayout(metadata);

		QHBoxLayout* actions = new QHBoxLayout();
		actions->setContentsMargins(0, 0, 0, 0);
		actions->setSpacing(8);

		if (!level.SourcePageUrl.isEmpty())
		{
			QPushButton* sourceButton = new QPushButton(QStringLiteral("Source"), body);
			sourceButton->setObjectName("MapCardSourceButton");
			sourceButton->setFixedSize(LauncherUi::Row::StatusActionWidth, LauncherUi::Row::StatusActionHeight);
			sourceButton->setToolTip(QStringLiteral("Open the publisher's preview and download page."));
			emit FocusableWidgetCreated(sourceButton);
			connect(sourceButton, &QPushButton::clicked, this, [sourcePageUrl = level.SourcePageUrl]() { QDesktopServices::openUrl(QUrl(sourcePageUrl)); });
			actions->addWidget(sourceButton);
		}

		actions->addStretch(1);
		QPushButton* actionButton = new QPushButton(body);
		ApplyLevelActionButtonState(*statusLabel, *actionButton, level);
		m_levelStatusLabels.insert(level.Id, statusLabel);
		m_levelActionButtons.insert(level.Id, actionButton);
		emit FocusableWidgetCreated(actionButton);

		connect(
		    actionButton,
		    &QPushButton::clicked,
		    this,
		    [this, content, level, actionButton]()
		    {
			    const QString actionIntent = actionButton->property("ActionIntent").toString();
			    if (actionIntent == QStringLiteral("run"))
			    {
				    StartLevel(content, level);
			    }
			    else if (actionIntent == QStringLiteral("sync"))
			    {
				    LauncherOperationRequest request = BuildLauncherOperationRequest(m_workspaceRoot, m_contentModel, m_settings, QStringLiteral("levels.sync"));
				    request.RequestedLevelIds = level.Id;
				    m_startOperation(std::move(request), QStringLiteral("Sync %1").arg(level.DisplayName));
			    }
			    else if (actionIntent == QStringLiteral("details"))
			    {
				    QMessageBox::information(&m_dialogParent, level.DisplayName, actionButton->toolTip());
			    }
		    });

		actions->addWidget(actionButton);
		bodyLayout->addLayout(actions);
		cardLayout->addWidget(body, 3);
		card->SetActivationButton(actionButton);
		grid.AddCard(card);
	}

	void LauncherSceneCatalog::ApplyLevelActionButtonState(QLabel& statusLabel, QPushButton& button, const LauncherLevelUiEntry& level)
	{
		ApplyInlineStatusLabel(statusLabel, level.Status, level.State);
		const bool bPreparing = !m_startingLevelRunId.isEmpty() && m_startingLevelId == level.Id;
		button.setObjectName("MapCardActionButton");
		button.setFixedSize(LauncherUi::Row::StatusActionWidth, LauncherUi::Row::StatusActionHeight);
		const bool bSyncAvailableSourceReady = !level.bReady && !level.bSourceReady;
		const bool bSyncAvailableCanSync = bSyncAvailableSourceReady && (level.bCanSync);
		const bool bSyncAvailable = bSyncAvailableCanSync && (level.bGameSupported);
		const bool bExplainBlocker = !level.bGameSupported || !level.bCanSelect;
		button.setProperty("ActionIntent", bPreparing ? QStringLiteral("none") : bSyncAvailable ? QStringLiteral("sync") : bExplainBlocker ? QStringLiteral("details") : QStringLiteral("run"));
		const bool bEditorMode = m_settings.RunMode() != QStringLiteral("game");
		const QString actionName = bEditorMode ? QStringLiteral("Open") : QStringLiteral("Run");
		button.setText(bPreparing ? QStringLiteral("Opening...") : bSyncAvailable ? QStringLiteral("Sync") : bExplainBlocker ? QStringLiteral("Details") : actionName);

		button.setAccessibleName(
		    bPreparing            ? QStringLiteral("Opening ") + level.DisplayName
		        : bSyncAvailable  ? QStringLiteral("Sync ") + level.DisplayName
		        : bExplainBlocker ? QStringLiteral("Why ") + level.DisplayName + QStringLiteral(" is unavailable")
		                          : actionName + QStringLiteral(" ") + level.DisplayName);

		button.setEnabled(m_startingLevelRunId.isEmpty());
		if (bPreparing)
		{
			button.setToolTip(QStringLiteral("Waiting for this level to start."));
		}
		else if (bSyncAvailable)
		{
			button.setToolTip(QStringLiteral("Download and verify this level's source assets."));
		}
		else if (bExplainBlocker)
		{
			button.setToolTip(level.UnsupportedReason);
		}
		else
		{
			button.setToolTip(
			    bEditorMode ? QStringLiteral("Open this level in the editor. Use Build and Cook first if required output files are missing.")
			                : QStringLiteral("Run this level in the standalone game. Use Build and Cook first if required output files are missing."));
		}
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

	bool LauncherSceneCatalog::SetLevelsSelected(const std::filesystem::path& contentRoot, const std::vector<std::string>& levelIds, bool bSelected, const QString& actionName)
	{
		std::string errorMessage;
		if (ProjectLevelCatalogFile::SetLevelsSelected(contentRoot, levelIds, bSelected, errorMessage))
		{
			return true;
		}

		QMessageBox::warning(&m_dialogParent, actionName + QStringLiteral(" could not update the level catalog"), QString::fromStdString(errorMessage));
		return false;
	}

	QVector<LauncherCleanTarget> LauncherSceneCatalog::BuildLevelCleanTargets(const LauncherContentSummary& content, const QString& levelId) const
	{
		QVector<LauncherCleanTarget> targets;
		for (const auto& path : BuildLevelCleanPaths(content.RootPath, levelId.toStdString()))
		{
			targets.push_back({QString::fromStdString(path.DisplayName), QString::fromStdString(path.Path.string()), QString::fromStdString(path.Detail)});
		}
		return targets;
	}

	void LauncherSceneCatalog::SyncAllLevels()
	{
		const LauncherContentSummary* content = m_contentModel.Content();
		if (content == nullptr)
		{
			return;
		}

		const LauncherLevelUiModel model = LauncherLevelUiModel::Build(*content);
		std::vector<std::string> levelIdsToSelect;
		QStringList levelIdsToSync;
		for (const LauncherLevelUiEntry& level : model.Levels)
		{
			if (level.Id == "Empty")
			{
				continue;
			}
			if (level.bCanSelect)
			{
				levelIdsToSelect.push_back(level.Id.toStdString());
				levelIdsToSync.push_back(level.Id);
			}
		}
		if (levelIdsToSync.isEmpty())
		{
			m_activityPanel->ShowMessage(QStringLiteral("No levels have a supported synchronization path."));
			return;
		}

		LauncherOperationRequest request = BuildLauncherOperationRequest(m_workspaceRoot, m_contentModel, m_settings, QStringLiteral("levels.sync"));
		request.RequestedLevelIds = levelIdsToSync.join(',');
		const QString runId = m_startOperation(std::move(request), QStringLiteral("Sync Available Levels"));
		m_pendingLevelSelectionUpdates.insert(runId, {content->RootPath, std::move(levelIdsToSelect), true});
	}

	void LauncherSceneCatalog::CleanAllLevels()
	{
		const LauncherContentSummary* content = m_contentModel.Content();
		if (content == nullptr)
		{
			return;
		}

		const LauncherLevelUiModel model = LauncherLevelUiModel::Build(*content);
		std::vector<std::string> levelIds;
		for (const LauncherLevelUiEntry& level : model.Levels)
		{
			if (level.Id != "Empty")
			{
				levelIds.push_back(level.Id.toStdString());
			}
		}

		QVector<LauncherCleanTarget> targets;
		try
		{
			targets = BuildLevelCleanTargets(*content);
		}
		catch (const std::exception& error)
		{
			QMessageBox::warning(&m_dialogParent, QStringLiteral("Level content could not be resolved"), QString::fromUtf8(error.what()));
			return;
		}
		if (targets.empty())
		{
			if (SetLevelsSelected(content->RootPath, levelIds, false, QStringLiteral("Clean All")))
			{
				RefreshLevelActionButtons();
				emit StateChanged();
			}
			return;
		}

		LauncherOperationRequest request = BuildLauncherOperationRequest(m_workspaceRoot, m_contentModel, m_settings, QStringLiteral("workspace.clean"));
		request.CleanTargets = targets;
		request.bConfirmClean = false;
		if (!ConfirmLauncherRunRequest(m_dialogParent, request))
		{
			return;
		}

		const QString runId = m_startOperation(std::move(request), QStringLiteral("Clean All Levels"));
		m_pendingLevelSelectionUpdates.insert(runId, {content->RootPath, levelIds, false});
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
			const bool bSelectableLevel = level.Id != "Empty" && level.bCanSelect;
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
				bHasExtractedLevelContent = !BuildLevelCleanTargets(*content).empty();
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
