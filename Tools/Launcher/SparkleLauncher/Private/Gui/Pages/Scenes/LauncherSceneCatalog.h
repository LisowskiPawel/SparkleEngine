#pragma once

#include "Gui/Operations/LauncherOperationRequest.h"
#include "Gui/Models/LauncherLevelUiModel.h"
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtWidgets/QWidget>
#include <filesystem>
#include <functional>

class QLabel;
class QPushButton;
class QVBoxLayout;
class QScrollArea;

namespace SparkleLauncher
{
	class LauncherActivityPanel;
	class LauncherContentModel;
	class LauncherSettings;
	class LauncherSceneView;
	struct LauncherContentSummary;
	struct LauncherLevelUiEntry;

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
		~LauncherSceneCatalog() override;
		void AddSceneSelection(QVBoxLayout& layout, QScrollArea& scrollArea);
		void SyncAllLevels();
		void CleanAllLevels();
		void Refresh();
		bool CompleteOperation(const QString& runId, const QString& operationId);
		void UpdateActionAvailability(QPushButton& cleanButton, QPushButton& runButton, bool bHasActionCleanTargets);

	signals:
		void FocusableWidgetCreated(QWidget* widget);

	private:
		void DispatchAction(const QString& contentId, const QString& levelId, LauncherLevelAction action);
		void StartLevel(const LauncherContentSummary& content, const LauncherLevelUiEntry& level);

		const std::filesystem::path& m_workspaceRoot;
		LauncherContentModel& m_contentModel;
		LauncherSettings& m_settings;
		LauncherActivityPanel* m_activityPanel;
		std::function<QString(LauncherOperationRequest, const QString&)> m_startOperation;
		QWidget& m_dialogParent;
		QString m_startingLevelRunId;
		QString m_startingLevelId;
		LauncherLevelUiModel m_levelModel;
		QPointer<LauncherSceneView> m_view;
		QPointer<QLabel> m_catalogMessage;
	};
}
