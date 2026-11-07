#pragma once

#include "Gui/Operations/LauncherOperationRequest.h"
#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QSet>
#include <QtWidgets/QWidget>
#include <filesystem>
#include <functional>

class QLabel;
class QPushButton;
class QVBoxLayout;

namespace SparkleLauncher
{
	class LauncherContentModel;
	class LauncherSettings;
	struct LauncherOperationResult;
	struct ThirdPartyDependencyUiEntry;
	struct ToolchainItemStatus;
	struct BuildFilesFreshnessStatus;
	struct BuildWorkspaceOperationRequest;
	struct BuildWorkspaceOperationPlan;
	enum class WorkspaceCompiler : std::uint8_t;

	class LauncherWorkspaceSetup final : public QObject
	{
		Q_OBJECT

	public:
		LauncherWorkspaceSetup(
		    const std::filesystem::path& workspaceRoot,
		    LauncherContentModel& contentModel,
		    LauncherSettings& settings,
		    std::function<QString(LauncherOperationRequest, const QString&)> startOperation,
		    QWidget& dialogParent);
		void AddBuildEnvironmentStatus(QVBoxLayout& layout, const QString& operationId);
		void TrackSourceDependencyRun(const LauncherOperationRequest& request, const QString& runId);
		void ResetWidgets();
		bool CompleteOperation(const QString& runId);

	signals:
		void FocusableWidgetCreated(QWidget* widget);
		void OperationSelected(const QString& operationId);
		void CompilerSelected(WorkspaceCompiler compiler);

	private:
		void AddSyncDependencies(QVBoxLayout& layout, bool optional);
		QPushButton* CreateSourceDependencyActionButton(const ThirdPartyDependencyUiEntry& dependency);
		void ApplySourceDependencyRowState(const ThirdPartyDependencyUiEntry& dependency, QLabel& statusLabel, QPushButton& button);
		void RefreshSourceDependencyRows();
		void SyncSourceDependency(const ThirdPartyDependencyUiEntry& dependency);
		void CleanSourceDependency(const ThirdPartyDependencyUiEntry& dependency);
		void InstallHostTool(const ToolchainItemStatus& item);
		QPushButton* CreateHostToolActionButton(const ToolchainItemStatus& item);
		void AddHostToolStatusRow(QVBoxLayout& layout, const ToolchainItemStatus& item);
		void AddBuildFilesStatus(QVBoxLayout& layout, const BuildFilesFreshnessStatus& freshness, bool offerRefresh);
		void AddSetupPrerequisiteStatus(QVBoxLayout& layout, const BuildWorkspaceOperationPlan& plan, bool isSourceSyncWorkflow);
		void AddWorkspaceSetupStatus(
		    QVBoxLayout& layout,
		    const QString& operationId,
		    const BuildWorkspaceOperationRequest& request,
		    const BuildWorkspaceOperationPlan& plan,
		    const QString& workspaceIdeName);
		void AddWorkspaceBuildIssues(QVBoxLayout& layout, const BuildWorkspaceOperationRequest& request, const BuildWorkspaceOperationPlan& plan);
		void AddBuildReadinessStatus(QVBoxLayout& layout, const BuildWorkspaceOperationPlan& plan);
		void AddCookEnvironmentStatus(QVBoxLayout& layout, const QString& operationId);
		QPushButton* CreateStatusActionButton(const QString& actionId, const QString& actionLabel, const QString& actionTitle, bool navigateInsteadOfRun = false);
		void TriggerActionDependencyRegenerate(const QString& actionId, const QString& actionTitle, bool navigateInsteadOfRun);

		const std::filesystem::path& m_workspaceRoot;
		LauncherContentModel& m_contentModel;
		LauncherSettings& m_settings;
		std::function<QString(LauncherOperationRequest, const QString&)> m_startOperation;
		QWidget& m_dialogParent;
		QHash<QString, QString> m_sourceDependencyRunIds;
		QHash<QString, QPointer<QLabel>> m_sourceDependencyStatusLabels;
		QHash<QString, QPointer<QPushButton>> m_sourceDependencyActionButtons;
		QSet<QString> m_cleaningSourceDependencyRunIds;
	};
}
