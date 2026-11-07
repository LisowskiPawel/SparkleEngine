#pragma once

#include "Gui/Style/LauncherIconLibrary.h"
#include "Gui/Models/LauncherOperationResult.h"
#include "Gui/Operations/LauncherOperationRequest.h"

#include <QtCore/QHash>
#include <QtCore/QPointer>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVector>
#include <QtWidgets/QMainWindow>

#include <cstdint>
#include <filesystem>

class QComboBox;
class QFrame;
class QPushButton;
class QStackedWidget;
class QVBoxLayout;

namespace SparkleLauncher
{
	struct LauncherOperationDescriptor;
	enum class WorkspaceCompiler : std::uint8_t;
	class LauncherBackend;
	class LauncherContentModel;
	class LauncherSettings;
	class LauncherActivityPanel;
	class LauncherGpuCrashPanel;
	class LauncherWorkflowPanel;
	class LauncherSceneCatalog;
	class LauncherWorkspaceSetup;

	class LauncherMainWindow final : public QMainWindow
	{
		Q_OBJECT

	public:
		LauncherMainWindow(std::filesystem::path workspaceRoot, LauncherContentModel& contentModel, LauncherSettings& settings, LauncherBackend& backend, QWidget* parent = nullptr);

		void SetStartupNotice(const QString& message);

	private slots:
		void RefreshContent();
		void HandleApplicationStateChanged(Qt::ApplicationState state);
		void RunSelectedOperation();
		void CleanSelectedOperation();
		void DisplayOperationStarted(const QString& runId, const QString& operationId, const QString& title);
		void AppendOperationOutput(const QString& runId, const QString& operationId, const QString& outputText);
		void UpdateOperationProgress(const QString& runId, const QString& operationId, const QString& phase, quint64 completed, quint64 total);
		void DisplayOperationFinished(const QString& runId, const QString& operationId, const QString& title, const LauncherOperationResult& result);

	private:
		void InitializeWorkflowPresentation(QWidget& parent);
		QWidget* CreateOptionsPanel(QWidget* parent);
		QWidget* CreateOptionsPage(const QString& operationId, QWidget* parent);
		QWidget* CreateFooterContextPanel(QWidget* parent);
		QComboBox* CreateContextCombo(void (LauncherSettings::*setter)(const QString&));
		void RefreshContextSelectors();
		void AddOptionsForOperation(QVBoxLayout& layout, const QString& operationId);
		void SelectWorkspaceCompiler(WorkspaceCompiler compiler);
		void SetControlsEnabled(bool bEnabled);
		void EnsureOptionsPage(const QString& operationId);
		void RebuildOptionsPages();
		void ScheduleUiRefresh(bool bRefreshContent);
		void ApplyScheduledUiRefresh();
		void RegisterFocusable(QWidget* widget);
		void ConfigureTabOrder();
		void UpdateRunAvailability();
		void UpdateWorkflowRunAvailability();
		const LauncherOperationDescriptor* FindOperationDescriptor(const QString& operationId) const;
		QString DisplayNameForOperation(const QString& operationId) const;
		bool OperationNeedsContent(const QString& operationId) const;
		void PromptForLauncherRestart();
		QString CreateRunId();
		QString StartOperation(LauncherOperationRequest request, const QString& title);
		void SetSelectedOperation(const QString& operationId);
		void ApplyVisualStyle();

		std::filesystem::path m_workspaceRoot;
		LauncherContentModel& m_contentModel;
		LauncherSettings& m_settings;
		LauncherBackend& m_backend;
		LauncherIconLibrary m_icons;
		LauncherWorkflowPanel* m_workflowPanel = nullptr;
		LauncherSceneCatalog* m_sceneCatalog = nullptr;
		LauncherWorkspaceSetup* m_workspaceSetup = nullptr;
		QVector<QPointer<QWidget>> m_tabOrderWidgets;
		QComboBox* m_runModeCombo = nullptr;
		QComboBox* m_buildConfigurationCombo = nullptr;
		QComboBox* m_workspaceCompilerCombo = nullptr;
		QComboBox* m_workspaceIdeCombo = nullptr;
		QComboBox* m_graphicsApiCombo = nullptr;
		QComboBox* m_captureProviderCombo = nullptr;
		QComboBox* m_shaderBackendCombo = nullptr;
		QStackedWidget* m_optionsStack = nullptr;
		QHash<QString, int> m_optionsPageByOperation;
		QPushButton* m_cleanButton = nullptr;
		QPushButton* m_runButton = nullptr;
		QFrame* m_actionMetaPanel = nullptr;
		LauncherActivityPanel* m_activityPanel = nullptr;
		LauncherGpuCrashPanel* m_gpuCrashPanel = nullptr;
		QString m_selectedOperationId;
		bool m_bIsRebuildingOptions = false;
		bool m_bIsApplyingUiRefresh = false;
		bool m_bUiRefreshQueued = false;
		bool m_bRefreshContentRequested = false;
		qint64 m_lastActivationRefreshMs = 0;
		int m_nextRunIndex = 0;
		QStringList m_pendingRestartRunIds;
	};
}
