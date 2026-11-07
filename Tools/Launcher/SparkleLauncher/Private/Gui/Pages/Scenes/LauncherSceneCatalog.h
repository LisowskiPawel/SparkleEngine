#pragma once

#include "Gui/Operations/LauncherOperationRequest.h"
#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtWidgets/QWidget>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

class QLabel;
class QPushButton;
class QVBoxLayout;

namespace SparkleLauncher
{
	class LauncherActivityPanel;
	class LauncherContentModel;
	class LauncherSettings;
	class ResponsiveCardGridWidget;
	struct LauncherContentSummary;
	struct LauncherLevelUiEntry;
	struct LauncherLevelUiModel;
	struct LauncherOperationResult;

	class LauncherSceneCatalog final : public QObject
	{
		Q_OBJECT

	public:
		LauncherSceneCatalog(
		    const std::filesystem::path& workspaceRoot,
		    LauncherContentModel& contentModel,
		    LauncherSettings& settings,
		    LauncherActivityPanel& activityPanel,
		    std::function<QString(LauncherOperationRequest, const QString&)> startOperation,
		    QWidget& dialogParent);
		void AddSceneSelection(QVBoxLayout& layout);
		void SyncAllLevels();
		void CleanAllLevels();
		void ResetWidgets();
		// Catalog changes refresh cards locally; launch completion still requires content rediscovery.
		bool CompleteOperation(const QString& runId, const QString& operationId, const QString& title, const LauncherOperationResult& result);
		void UpdateActionAvailability(QPushButton& cleanButton, QPushButton& runButton, bool bHasActionCleanTargets);

	signals:
		void FocusableWidgetCreated(QWidget* widget);
		void StateChanged();

	private:
		struct PendingLevelSelectionUpdate
		{
			std::filesystem::path ContentRoot;
			std::vector<std::string> LevelIds;
			bool bSelected = false;
		};

		void AddLevelContentGroups(QVBoxLayout& layout);
		void AddSyncLevelRows(QVBoxLayout& layout, const LauncherContentSummary& content, const LauncherLevelUiModel& model);
		void AddSyncLevelRow(ResponsiveCardGridWidget& grid, const LauncherContentSummary& content, const LauncherLevelUiEntry& level);
		void ApplyLevelActionButtonState(QLabel& statusLabel, QPushButton& button, const LauncherLevelUiEntry& level);
		void RefreshLevelActionButtons();
		bool SetLevelsSelected(const std::filesystem::path& contentRoot, const std::vector<std::string>& levelIds, bool bSelected, const QString& actionName);
		QVector<LauncherCleanTarget> BuildLevelCleanTargets(const LauncherContentSummary& content, const QString& levelId = QString()) const;
		LauncherLevelUiModel BuildLevelUiModel() const;
		void StartLevel(const LauncherContentSummary& content, const LauncherLevelUiEntry& level);

		const std::filesystem::path& m_workspaceRoot;
		LauncherContentModel& m_contentModel;
		LauncherSettings& m_settings;
		LauncherActivityPanel* m_activityPanel;
		std::function<QString(LauncherOperationRequest, const QString&)> m_startOperation;
		QWidget& m_dialogParent;
		QString m_startingLevelRunId;
		QString m_startingLevelId;
		QHash<QString, PendingLevelSelectionUpdate> m_pendingLevelSelectionUpdates;
		QHash<QString, QPointer<QLabel>> m_levelStatusLabels;
		QHash<QString, QPointer<QPushButton>> m_levelActionButtons;
	};
}
