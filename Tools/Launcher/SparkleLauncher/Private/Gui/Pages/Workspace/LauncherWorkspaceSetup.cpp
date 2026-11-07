#include "Gui/Pages/Workspace/LauncherWorkspaceSetup.h"
#include "Gui/Widgets/LauncherOptionWidgets.h"
#include "Gui/Operations/LauncherOperationConfirmation.h"
#include <utility>

#include "Gui/Widgets/LauncherActionWidgets.h"
#include "Gui/Models/LauncherCleanUiModel.h"
#include "Gui/Models/LauncherDependencyUiModel.h"
#include "Gui/Widgets/LauncherLayoutWidgets.h"
#include "Gui/Models/LauncherOperationRequestFactory.h"
#include "Gui/Operations/LauncherOperationRequestMapping.h"
#include "Gui/Models/LauncherContentModel.h"
#include "Gui/App/LauncherSettings.h"
#include "Gui/Models/LauncherToolchainUiModel.h"
#include "Gui/Style/LauncherUiDesign.h"
#include "Gui/Models/LauncherUiModel.h"
#include "Gui/Models/LauncherWorkflowCatalog.h"

#include "SparkleLauncher/BuildWorkspaceOperations.h"
#include "SparkleLauncher/CookOperations.h"
#include "SparkleLauncher/MaintenanceOperations.h"

#include <QtCore/QSignalBlocker>
#include <QtCore/QStringList>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSizePolicy>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include <cstdint>
#include <filesystem>
#include <system_error>
#include <vector>

namespace SparkleLauncher
{
	static QString CombineStatusDetail(const QString& first, const QString& second)
	{
		if (first.isEmpty())
		{
			return second;
		}
		if (second.isEmpty())
		{
			return first;
		}
		return first + " | " + second;
	}

	static QString BuildFilesExpectedAction(const BuildFilesFreshnessStatus& freshness)
	{
		switch (freshness.State)
		{
			case BuildFilesFreshnessState::GeneratorMismatch:
			case BuildFilesFreshnessState::FeatureSetMismatch:
				return "Next action: clean Build Outputs or choose a different build directory, then run Generate Build Files again.";
			case BuildFilesFreshnessState::BuildDirectoryMissing:
			case BuildFilesFreshnessState::CMakeCacheMissing:
			case BuildFilesFreshnessState::SolutionMissing:
			case BuildFilesFreshnessState::FreshnessStampMissing:
			case BuildFilesFreshnessState::FreshnessStampMismatch:
			case BuildFilesFreshnessState::SourceListChanged:
			case BuildFilesFreshnessState::BuildInputChanged:
				return "Next action: run Generate Build Files to refresh generated CMake and IDE state.";

			case BuildFilesFreshnessState::Current:
			case BuildFilesFreshnessState::Unsupported:
				return {};
		}
		return {};
	}

	LauncherWorkspaceSetup::LauncherWorkspaceSetup(
	    const std::filesystem::path& workspaceRoot,
	    LauncherContentModel& contentModel,
	    LauncherSettings& settings,
	    std::function<QString(LauncherOperationRequest, const QString&)> startOperation,
	    QWidget& dialogParent) :
	    QObject(&dialogParent),
	    m_workspaceRoot(workspaceRoot),
	    m_contentModel(contentModel),
	    m_settings(settings),
	    m_startOperation(std::move(startOperation)),
	    m_dialogParent(dialogParent)
	{
	}

	void LauncherWorkspaceSetup::ResetWidgets()
	{
		m_sourceDependencyStatusLabels.clear();
		m_sourceDependencyActionButtons.clear();
	}

	bool LauncherWorkspaceSetup::CompleteOperation(const QString& runId)
	{
		bool changed = false;
		for (auto dependencyRun = m_sourceDependencyRunIds.begin(); dependencyRun != m_sourceDependencyRunIds.end();)
		{
			if (dependencyRun.value() == runId)
			{
				changed = true;
				dependencyRun = m_sourceDependencyRunIds.erase(dependencyRun);
			}
			else
			{
				++dependencyRun;
			}
		}
		m_cleaningSourceDependencyRunIds.remove(runId);
		if (changed)
		{
			RefreshSourceDependencyRows();
		}
		return changed;
	}

	static constexpr int kSpaceSmall = LauncherUi::Space::Small;

	static QString CompactToolchainDetail(const ToolchainItemStatus& item)
	{
		if (item.State == ToolchainItemState::Found && !item.Path.empty())
		{
			return QString::fromStdString(item.Path.string());
		}
		if (!item.Detail.empty())
		{
			return QString::fromStdString(item.Detail);
		}
		if (!item.Path.empty())
		{
			return QString::fromStdString(item.Path.string());
		}
		return QString();
	}

	void LauncherWorkspaceSetup::InstallHostTool(const ToolchainItemStatus& item)
	{
		LauncherOperationRequest request = BuildLauncherOperationRequest(m_workspaceRoot, m_contentModel, m_settings, "workspace.install-host-tool");
		request.HostToolId = QString::fromStdString(item.Id);
		m_startOperation(std::move(request), "Install " + QString::fromStdString(item.DisplayName));
	}

	QPushButton* LauncherWorkspaceSetup::CreateHostToolActionButton(const ToolchainItemStatus& item)
	{
		const std::optional<WorkspaceCompiler> compiler = item.Compiler;
		const bool selected = compiler.has_value() && *compiler == ResolveSelectedWorkspaceCompiler(m_settings);
		if (item.State == ToolchainItemState::Found && (!compiler.has_value() || selected))
		{
			return nullptr;
		}
		if (item.State != ToolchainItemState::Found && !item.CanInstall)
		{
			return nullptr;
		}

		const bool install = item.State != ToolchainItemState::Found;
		const QString label = install ? QStringLiteral("Install") : QStringLiteral("Use");
		const QString displayName = QString::fromStdString(item.DisplayName);
		QPushButton* button = new QPushButton(&m_dialogParent);
		ApplyStatusActionButtonPresentation(*button, label, install ? QStringLiteral("warning") : QStringLiteral("neutral"));
		button->setAccessibleName(label + " " + displayName);
		button->setToolTip(install ? "Install " + displayName + ". It becomes selectable after detection confirms the installation." : "Select " + displayName + " for launcher builds.");
		emit FocusableWidgetCreated(button);

		connect(
		    button,
		    &QPushButton::clicked,
		    this,
		    [this, item, compiler, install]()
		    {
			    if (install)
			    {
				    InstallHostTool(item);
			    }
			    else if (compiler.has_value())
			    {
				    emit CompilerSelected(*compiler);
			    }
		    });

		return button;
	}

	void LauncherWorkspaceSetup::AddHostToolStatusRow(QVBoxLayout& layout, const ToolchainItemStatus& item)
	{
		AddStatusRow(
		    layout,
		    QString::fromStdString(item.DisplayName),
		    ToolchainStatusText(item.State, item.Required),
		    CompactToolchainDetail(item),
		    ToolchainStatusState(item.State, item.Required),
		    CreateHostToolActionButton(item));
	}

	void LauncherWorkspaceSetup::AddBuildFilesStatus(QVBoxLayout& layout, const BuildFilesFreshnessStatus& freshness, bool offerRefresh)
	{
		AddStatusRow(
		    layout,
		    "Build files",
		    freshness.Current ? "Ready" : "Needs refresh",
		    CombineStatusDetail(QString::fromStdString(freshness.Summary), BuildFilesExpectedAction(freshness)),
		    freshness.Current ? "ok" : "warning",
		    !offerRefresh || freshness.Current ? nullptr : CreateStatusActionButton("workspace.generate-build-files", "Generate", "Generate Build Files"));
	}

	void LauncherWorkspaceSetup::AddSetupPrerequisiteStatus(QVBoxLayout& layout, const BuildWorkspaceOperationPlan& plan, bool isSourceSyncWorkflow)
	{
		const QString configurePrerequisiteDetail = !plan.CanRun && !plan.ReadinessMessages.empty() ? QString::fromStdString(plan.ReadinessMessages.back()) : QString();

		if (!plan.Toolchain.RequiredToolsAvailable)
		{
			AddStatusRow(layout, "Action needed", "Blocked", RequiredToolProblemSummary(plan.Toolchain), "bad", CreateStatusActionButton("workspace.sync-code", "Review", "Review Sync Code", true));
		}
		else if (!plan.CanRun && !configurePrerequisiteDetail.isEmpty())
		{
			AddStatusRow(
			    layout,
			    isSourceSyncWorkflow ? "Missing host prerequisite" : "Renderer prerequisites",
			    "Blocked",
			    configurePrerequisiteDetail,
			    "bad",
			    CreateStatusActionButton("workspace.sync-code", "Review", "Review Sync Code", true));
		}
	}

	void LauncherWorkspaceSetup::AddWorkspaceSetupStatus(
	    QVBoxLayout& layout,
	    const QString& operationId,
	    const BuildWorkspaceOperationRequest& request,
	    const BuildWorkspaceOperationPlan& plan,
	    const QString& workspaceIdeName)
	{
		const bool isSourceSyncWorkflow = operationId == "workspace.sync-code";

		QVBoxLayout* machineLayout = AddOptionGroup(layout, "Dependencies", QString());

		const auto addToolchainItems = [this, machineLayout, &plan](bool requiredOnly)
		{
			for (const ToolchainItemStatus& item : plan.Toolchain.Items)
			{
				if (item.Required != requiredOnly)
				{
					continue;
				}
				AddHostToolStatusRow(*machineLayout, item);
			}
		};

		addToolchainItems(true);

		AddStatusRow(
		    *machineLayout,
		    "Selected IDE",
		    workspaceIdeName,
		    request.PreferredIde == WorkspaceIde::Rider
		        ? (plan.Toolchain.RiderPath.empty() ? "Rider executable was not found." : QString::fromStdString(plan.Toolchain.RiderPath.string()))
		        : (plan.Toolchain.VisualStudioIdePath.empty() ? "Visual Studio IDE was not found." : QString::fromStdString(plan.Toolchain.VisualStudioIdePath.string())),
		    request.PreferredIde == WorkspaceIde::Rider ? (plan.Toolchain.RiderPath.empty() ? "warning" : "ok") : (plan.Toolchain.VisualStudioIdePath.empty() ? "warning" : "ok"));

		if (!isSourceSyncWorkflow)
		{
			AddBuildFilesStatus(*machineLayout, plan.Freshness, false);
		}

		AddSetupPrerequisiteStatus(*machineLayout, plan, isSourceSyncWorkflow);

		machineLayout->addSpacing(kSpaceSmall);
		machineLayout->addWidget(CreateSectionLabel("Optional host tools"));
		addToolchainItems(false);
		if (isSourceSyncWorkflow)
		{
			machineLayout->addSpacing(kSpaceSmall);
			machineLayout->addWidget(CreateSectionLabel("Source dependencies"));
			AddSyncDependencies(*machineLayout, false);
			machineLayout->addSpacing(kSpaceSmall);
			machineLayout->addWidget(CreateSectionLabel("Optional source dependencies"));
			AddSyncDependencies(*machineLayout, true);
		}
	}

	void LauncherWorkspaceSetup::AddWorkspaceBuildIssues(QVBoxLayout& layout, const BuildWorkspaceOperationRequest& request, const BuildWorkspaceOperationPlan& plan)
	{
		QVBoxLayout* issueLayout = nullptr;

		const auto ensureIssueLayout = [&]() -> QVBoxLayout*
		{
			if (issueLayout == nullptr)
			{
				issueLayout = AddOptionGroup(layout, "Setup needed", "Only prerequisites that need attention are shown here. Resolve them once, then run the selected build.");
			}
			return issueLayout;
		};

		for (const ToolchainItemStatus& item : plan.Toolchain.Items)
		{
			if (!item.Required || item.State == ToolchainItemState::Found)
			{
				continue;
			}

			AddHostToolStatusRow(*ensureIssueLayout(), item);
		}

		if (HasIncompleteEnabledSourceDependencies(plan))
		{
			AddStatusRow(
			    *ensureIssueLayout(),
			    "Source dependencies",
			    "Blocked",
			    "One or more enabled repository dependencies are incomplete.",
			    "bad",
			    CreateStatusActionButton("workspace.sync-code", "Review", "Review Sync Code", true));
		}

		if (!plan.CanRun && issueLayout == nullptr && !request.SelectedScopes.empty())
		{
			const QString detail = plan.ReadinessMessages.empty() ? QStringLiteral("The selected build cannot run with the current workspace configuration.")
			                                                      : QString::fromStdString(plan.ReadinessMessages.back());

			AddStatusRow(*ensureIssueLayout(), "Build selection", "Unavailable", detail, "bad");
		}
	}

	void LauncherWorkspaceSetup::AddBuildReadinessStatus(QVBoxLayout& layout, const BuildWorkspaceOperationPlan& plan)
	{
		QVBoxLayout* buildLayout = AddOptionGroup(layout, "Readiness", QString());

		AddStatusRow(
		    *buildLayout,
		    "Required tools",
		    plan.Toolchain.RequiredToolsAvailable ? "Ready" : "Blocked",
		    plan.Toolchain.RequiredToolsAvailable ? BuildGeneratorSummary(plan.Toolchain) : RequiredToolProblemSummary(plan.Toolchain),
		    plan.Toolchain.RequiredToolsAvailable ? "ok" : "bad",
		    plan.Toolchain.RequiredToolsAvailable ? nullptr : CreateStatusActionButton("workspace.sync-code", "Review", "Review Sync Code", true));

		AddBuildFilesStatus(*buildLayout, plan.Freshness, true);
	}

	void LauncherWorkspaceSetup::AddBuildEnvironmentStatus(QVBoxLayout& layout, const QString& operationId)
	{
		if (operationId.startsWith("cook.") && operationId != "cook.tools.prepare")
		{
			AddCookEnvironmentStatus(layout, operationId);
			return;
		}

		const BuildWorkspaceOperationRequest request = BuildWorkspacePlanRequest(m_workspaceRoot, m_contentModel, m_settings);

		const BuildWorkspaceOperationPlan plan = PlanBuildWorkspaceOperation(operationId.toStdString(), request);
		const QString workspaceIdeName = ResolveSelectedWorkspaceIdeName(m_settings);
		if (operationId == "workspace.sync-code" || operationId == "workspace.generate-build-files")
		{
			AddWorkspaceSetupStatus(layout, operationId, request, plan, workspaceIdeName);
		}
		else if (operationId == "workspace.build")
		{
			AddWorkspaceBuildIssues(layout, request, plan);
		}
		else if (operationId.startsWith("workspace.build") || operationId == "cook.tools.prepare" || operationId == "launcher.build.self")
		{
			AddBuildReadinessStatus(layout, plan);
		}
	}

	void LauncherWorkspaceSetup::AddCookEnvironmentStatus(QVBoxLayout& layout, const QString& operationId)
	{
		const LauncherOperationRequest operationRequest = BuildLauncherOperationRequest(m_workspaceRoot, m_contentModel, m_settings, operationId);
		const CookOperationPlan cookPlan = PlanCookOperation(operationId.toStdString(), LauncherOperationRequestMapping::Cook(operationRequest));
		QVBoxLayout* issueLayout = nullptr;

		const auto ensureIssueLayout = [&]() -> QVBoxLayout*
		{
			if (issueLayout == nullptr)
			{
				issueLayout = AddOptionGroup(layout, "Setup needed", "Only prerequisites that need attention are shown here. Resolve them once, then cook the selected outputs.");
			}
			return issueLayout;
		};

		QStringList missingCookTools;
		for (const std::filesystem::path& toolPath : cookPlan.RequiredToolPaths)
		{
			std::error_code errorCode;
			if (!std::filesystem::exists(toolPath, errorCode) || errorCode)
			{
				missingCookTools.push_back(QString::fromStdString(toolPath.filename().string()));
			}
		}

		if (!missingCookTools.empty())
		{
			AddStatusRow(
			    *ensureIssueLayout(),
			    "Cooking tools",
			    "Missing",
			    "Sync products or build the required tools: " + missingCookTools.join(", ") + ".",
			    "bad",
			    CreateStatusActionButton("cook.tools.prepare", "Build", "Build Cooking Tools"));
		}

		if (!cookPlan.CanRun && issueLayout == nullptr && !cookPlan.Request.SelectedScopes.empty())
		{
			const QString detail = cookPlan.ReadinessMessages.empty() ? QStringLiteral("The selected cooking stages cannot run with the current workspace configuration.")
			                                                          : QString::fromStdString(cookPlan.ReadinessMessages.back());

			QWidget* recoveryAction = detail.contains("Cook tool", Qt::CaseInsensitive) ? CreateStatusActionButton("cook.tools.prepare", "Build", "Build Cooking Tools") : nullptr;
			AddStatusRow(*ensureIssueLayout(), "Cook selection", "Unavailable", detail, "bad", recoveryAction);
		}
	}

	QPushButton* LauncherWorkspaceSetup::CreateStatusActionButton(const QString& actionId, const QString& actionLabel, const QString& actionTitle, bool navigateInsteadOfRun)
	{
		QPushButton* button = new QPushButton(&m_dialogParent);
		ApplyStatusActionButtonPresentation(*button, actionLabel, "warning");
		button->setAccessibleName(actionTitle);
		button->setToolTip(actionTitle + ".");
		emit FocusableWidgetCreated(button);
		connect(button, &QPushButton::clicked, this, [this, actionId, actionTitle, navigateInsteadOfRun]() { TriggerActionDependencyRegenerate(actionId, actionTitle, navigateInsteadOfRun); });
		return button;
	}

	void LauncherWorkspaceSetup::TriggerActionDependencyRegenerate(const QString& actionId, const QString& actionTitle, bool navigateInsteadOfRun)
	{
		if (navigateInsteadOfRun)
		{
			emit OperationSelected(actionId);
			return;
		}

		LauncherOperationRequest request = BuildLauncherOperationRequest(m_workspaceRoot, m_contentModel, m_settings, actionId);
		if (!ConfirmLauncherRunRequest(m_dialogParent, request))
		{
			return;
		}

		m_startOperation(std::move(request), actionTitle);
	}
}
