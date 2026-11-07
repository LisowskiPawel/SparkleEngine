#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include "Gui/Pages/Scenes/LauncherSceneCatalog.h"
#include "Gui/Pages/Workspace/LauncherWorkspaceSetup.h"
#include "Gui/Widgets/LauncherOptionWidgets.h"
#include "Gui/Operations/LauncherOperationConfirmation.h"
#include "Gui/Shell/LauncherMainWindow.h"

#include "Gui/Shell/LauncherActivityPanel.h"
#include "Gui/Shell/LauncherWorkflowPanel.h"
#include "Gui/Widgets/LauncherActionWidgets.h"
#include "Gui/App/LauncherBackend.h"
#include "Gui/Models/LauncherCleanUiModel.h"
#include "Gui/Models/LauncherDependencyUiModel.h"
#include "Gui/Style/LauncherIconLibrary.h"
#include "Gui/Widgets/LauncherLayoutWidgets.h"
#include "Gui/Models/LauncherLevelUiModel.h"
#include "Gui/Models/LauncherOperationRequestFactory.h"
#include "Gui/Operations/LauncherOperationRequestMapping.h"
#include "Gui/Models/LauncherContentModel.h"
#include "Gui/App/LauncherSettings.h"
#include "Gui/Models/LauncherToolchainUiModel.h"
#include "Gui/Style/LauncherUiDesign.h"
#include "Gui/Models/LauncherUiModel.h"
#include "Gui/Style/LauncherVisualStyle.h"

#include "SparkleLauncher/BuildWorkspaceOperations.h"
#include "SparkleLauncher/CookOperations.h"
#include "SparkleLauncher/MaintenanceOperations.h"

#include "Core/Public/Diagnostics/Error.h"

#include <QtCore/QSignalBlocker>
#include <QtCore/QCoreApplication>
#include <QtCore/QPointer>
#include <QtCore/QProcess>
#include <QtCore/QRegularExpression>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtCore/Qt>
#include <QtCore/QDateTime>
#include <QtGui/QBrush>
#include <QtGui/QClipboard>
#include <QtGui/QColor>
#include <QtGui/QDesktopServices>
#include <QtGui/QFont>
#include <QtGui/QGuiApplication>
#include <QtGui/QImage>
#include <QtGui/QKeyEvent>
#include <QtGui/QKeySequence>
#include <QtGui/QMouseEvent>
#include <QtGui/QPixmap>
#include <QtGui/QResizeEvent>
#include <QtGui/QTextCursor>
#include <QtGui/QTextDocument>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QSizePolicy>
#include <QtWidgets/QStyle>
#include <QtWidgets/QWidget>
#include <QtCore/QUrl>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <system_error>
#include <utility>

namespace SparkleLauncher
{
	static constexpr int kLauncherMinimumWidth = LauncherUi::Window::MinimumWidth;
	static constexpr int kLauncherMinimumHeight = LauncherUi::Window::MinimumHeight;
	static constexpr int kLauncherInitialWidth = LauncherUi::Window::InitialWidth;
	static constexpr int kLauncherInitialHeight = LauncherUi::Window::InitialHeight;
	static constexpr qint64 kActivationRefreshIntervalMs = 1500;

	static QString FirstBlockingReadinessMessage(const std::vector<std::string>& readinessMessages)
	{
		for (const std::string& message : readinessMessages)
		{
			if (!message.empty())
			{
				return QString::fromStdString(message);
			}
		}

		return "This workflow is currently blocked.";
	}

	static void RestoreScrollBarPosition(QScrollBar* scrollBar, int position, QObject* context)
	{
		if (scrollBar == nullptr)
		{
			return;
		}
		QPointer<QScrollBar> guardedScrollBar(scrollBar);

		QTimer::singleShot(
		    0,
		    context,
		    [guardedScrollBar, position]()
		    {
			    if (guardedScrollBar != nullptr)
			    {
				    guardedScrollBar->setValue(position);
			    }
		    });
	}

	LauncherMainWindow::LauncherMainWindow(std::filesystem::path workspaceRoot, LauncherContentModel& contentModel, LauncherSettings& settings, LauncherBackend& backend, QWidget* parent) :
	    QMainWindow(parent),
	    m_workspaceRoot(std::move(workspaceRoot)),
	    m_contentModel(contentModel),
	    m_settings(settings),
	    m_backend(backend)
	{
		setWindowTitle("Sparkle Launcher");
		setMinimumSize(kLauncherMinimumWidth, kLauncherMinimumHeight);
		resize(kLauncherInitialWidth, kLauncherInitialHeight);
		m_icons.Load();
		const QIcon applicationIcon = m_icons.ApplicationIcon();
		QGuiApplication::setWindowIcon(applicationIcon);
		setWindowIcon(applicationIcon);

		QWidget* centralWidget = new QWidget(this);
		QVBoxLayout* rootLayout = new QVBoxLayout(centralWidget);
		rootLayout->setContentsMargins(0, 0, 0, 0);
		rootLayout->setSpacing(0);

		// The workflow panel invokes its page factory during construction.
		InitializeWorkflowPresentation(*centralWidget);

		m_workflowPanel = new LauncherWorkflowPanel(
		    m_icons,
		    [this](const QString& operationId) { return DisplayNameForOperation(operationId); },
		    [this](QWidget* widget) { RegisterFocusable(widget); },
		    [this](QWidget* parent) { return CreateOptionsPanel(parent); },
		    centralWidget);

		connect(m_workflowPanel, &LauncherWorkflowPanel::OperationSelected, this, &LauncherMainWindow::SetSelectedOperation);
		rootLayout->addWidget(m_workflowPanel, 1);

		rootLayout->addWidget(m_activityPanel, 0);
		rootLayout->addWidget(CreateFooterContextPanel(centralWidget), 0);
		RefreshContextSelectors();
		setCentralWidget(centralWidget);
		const QString initialOperationId = m_workflowPanel->InitialOperationId();
		if (!initialOperationId.isEmpty())
		{
			SetSelectedOperation(initialOperationId);
		}

		ConfigureTabOrder();
		ApplyVisualStyle();
		ApplyNativeDarkTitleBar(*this);

		connect(&m_contentModel, &LauncherContentModel::ContentChanged, this, [this]() { ScheduleUiRefresh(false); });
		connect(&m_contentModel, &LauncherContentModel::ContentDiscoveryFailed, this, &LauncherMainWindow::SetStartupNotice);
		connect(&m_settings, &LauncherSettings::SettingsChanged, this, [this]() { ScheduleUiRefresh(false); });
		connect(&m_backend, &LauncherBackend::OperationStarted, this, &LauncherMainWindow::DisplayOperationStarted);
		connect(&m_backend, &LauncherBackend::OperationOutputReceived, this, &LauncherMainWindow::AppendOperationOutput);
		connect(&m_backend, &LauncherBackend::OperationProgressReceived, this, &LauncherMainWindow::UpdateOperationProgress);
		connect(&m_backend, &LauncherBackend::OperationFinished, this, &LauncherMainWindow::DisplayOperationFinished);
		connect(qApp, &QGuiApplication::applicationStateChanged, this, &LauncherMainWindow::HandleApplicationStateChanged);

		QTimer::singleShot(0, this, &LauncherMainWindow::RefreshContent);
	}

	void LauncherMainWindow::InitializeWorkflowPresentation(QWidget& parent)
	{
		m_activityPanel = new LauncherActivityPanel(m_icons, [this](QWidget* widget) { RegisterFocusable(widget); }, [this](const QString& runId) { m_backend.CancelOperation(runId); }, &parent);

		m_sceneCatalog = new LauncherSceneCatalog(
		    m_workspaceRoot,
		    m_contentModel,
		    m_settings,
		    *m_activityPanel,
		    [this](LauncherOperationRequest request, const QString& title) { return StartOperation(std::move(request), title); },
		    *this);

		m_workspaceSetup = new LauncherWorkspaceSetup(
		    m_workspaceRoot,
		    m_contentModel,
		    m_settings,
		    [this](LauncherOperationRequest request, const QString& title) { return StartOperation(std::move(request), title); },
		    *this);

		connect(m_sceneCatalog, &LauncherSceneCatalog::FocusableWidgetCreated, this, &LauncherMainWindow::RegisterFocusable);
		connect(m_sceneCatalog, &LauncherSceneCatalog::StateChanged, this, &LauncherMainWindow::UpdateRunAvailability);
		connect(m_workspaceSetup, &LauncherWorkspaceSetup::FocusableWidgetCreated, this, &LauncherMainWindow::RegisterFocusable);
		connect(m_workspaceSetup, &LauncherWorkspaceSetup::OperationSelected, this, &LauncherMainWindow::SetSelectedOperation);
		connect(m_workspaceSetup, &LauncherWorkspaceSetup::CompilerSelected, this, &LauncherMainWindow::SelectWorkspaceCompiler);
	}

	void LauncherMainWindow::HandleApplicationStateChanged(Qt::ApplicationState state)
	{
		if (state != Qt::ApplicationActive)
		{
			return;
		}

		const qint64 nowMs = QDateTime::currentMSecsSinceEpoch();
		if (m_lastActivationRefreshMs != 0 && nowMs - m_lastActivationRefreshMs < kActivationRefreshIntervalMs)
		{
			return;
		}

		m_lastActivationRefreshMs = nowMs;
		ScheduleUiRefresh(true);
	}

	void LauncherMainWindow::SetStartupNotice(const QString& message)
	{
		if (!message.isEmpty())
		{
			UpdateRunAvailability();
		}
	}

	void LauncherMainWindow::RefreshContent()
	{
		ScheduleUiRefresh(true);
	}

	void LauncherMainWindow::ScheduleUiRefresh(bool bRefreshContent)
	{
		m_bRefreshContentRequested = m_bRefreshContentRequested || bRefreshContent;
		if (m_bIsApplyingUiRefresh || m_bUiRefreshQueued)
		{
			return;
		}

		m_bUiRefreshQueued = true;
		QTimer::singleShot(0, this, &LauncherMainWindow::ApplyScheduledUiRefresh);
	}

	void LauncherMainWindow::ApplyScheduledUiRefresh()
	{
		if (m_bIsApplyingUiRefresh)
		{
			return;
		}

		m_bUiRefreshQueued = false;
		const bool bRefreshContent = m_bRefreshContentRequested;
		m_bRefreshContentRequested = false;
		m_bIsApplyingUiRefresh = true;

		if (bRefreshContent)
		{
			m_contentModel.Refresh(m_workspaceRoot);
			RefreshContextSelectors();
		}

		RebuildOptionsPages();
		UpdateRunAvailability();
		m_bIsApplyingUiRefresh = false;
		if (m_bRefreshContentRequested)
		{
			ScheduleUiRefresh(false);
		}
	}

	void LauncherMainWindow::SetControlsEnabled(bool bEnabled)
	{
		if (m_cleanButton != nullptr)
		{
			m_cleanButton->setEnabled(bEnabled);
		}
		if (m_runButton != nullptr)
		{
			m_runButton->setEnabled(bEnabled);
		}
		if (m_optionsStack != nullptr)
		{
			m_optionsStack->setVisible(bEnabled);
		}
	}

	void LauncherMainWindow::RebuildOptionsPages()
	{
		if (m_optionsStack == nullptr || m_bIsRebuildingOptions)
		{
			return;
		}

		int preservedVerticalScroll = 0;
		int preservedHorizontalScroll = 0;
		if (QScrollArea* currentScrollArea = qobject_cast<QScrollArea*>(m_optionsStack->currentWidget()))
		{
			if (QScrollBar* verticalScrollBar = currentScrollArea->verticalScrollBar())
			{
				preservedVerticalScroll = verticalScrollBar->value();
			}
			if (QScrollBar* horizontalScrollBar = currentScrollArea->horizontalScrollBar())
			{
				preservedHorizontalScroll = horizontalScrollBar->value();
			}
		}

		m_bIsRebuildingOptions = true;

		while (m_optionsStack->count() > 0)
		{
			QWidget* page = m_optionsStack->widget(0);
			m_optionsStack->removeWidget(page);
			page->deleteLater();
		}
		m_optionsPageByOperation.clear();
		m_sceneCatalog->ResetWidgets();
		m_workspaceSetup->ResetWidgets();

		EnsureOptionsPage(m_selectedOperationId);
		if (m_optionsPageByOperation.contains(m_selectedOperationId))
		{
			m_optionsStack->setCurrentIndex(m_optionsPageByOperation.value(m_selectedOperationId));
			if (QScrollArea* rebuiltScrollArea = qobject_cast<QScrollArea*>(m_optionsStack->currentWidget()))
			{
				RestoreScrollBarPosition(rebuiltScrollArea->verticalScrollBar(), preservedVerticalScroll, this);
				RestoreScrollBarPosition(rebuiltScrollArea->horizontalScrollBar(), preservedHorizontalScroll, this);
			}
		}

		ConfigureTabOrder();
		m_bIsRebuildingOptions = false;
	}

	void LauncherMainWindow::EnsureOptionsPage(const QString& operationId)
	{
		if (m_optionsStack == nullptr || operationId.isEmpty() || m_optionsPageByOperation.contains(operationId))
		{
			return;
		}

		const int pageIndex = m_optionsStack->addWidget(CreateOptionsPage(operationId, m_optionsStack));
		m_optionsPageByOperation.insert(operationId, pageIndex);
		m_optionsStack->setCurrentIndex(pageIndex);
	}

	void LauncherMainWindow::RegisterFocusable(QWidget* widget)
	{
		if (widget == nullptr)
		{
			return;
		}

		widget->setFocusPolicy(Qt::StrongFocus);
		m_tabOrderWidgets.removeAll(nullptr);
		m_tabOrderWidgets.push_back(widget);
	}

	void LauncherMainWindow::ConfigureTabOrder()
	{
		QWidget* previousWidget = nullptr;
		for (const QPointer<QWidget>& guardedWidget : m_tabOrderWidgets)
		{
			QWidget* widget = guardedWidget.data();
			if (widget == nullptr)
			{
				continue;
			}

			if (previousWidget != nullptr)
			{
				setTabOrder(previousWidget, widget);
			}
			previousWidget = widget;
		}
	}

	void LauncherMainWindow::UpdateRunAvailability()
	{
		if (m_runButton == nullptr || m_cleanButton == nullptr)
		{
			return;
		}

		if (m_selectedOperationId.isEmpty())
		{
			const QString reason = "Select a workflow before running.";
			ApplyActionAvailability(*m_runButton, false, reason);
			ApplyActionAvailability(*m_cleanButton, false, "Select a workflow before cleaning generated outputs.");
			return;
		}

		m_runButton->setVisible(true);
		const bool bIsLevelCatalog = m_selectedOperationId == LauncherScenesOperationId();
		m_cleanButton->setVisible(bIsLevelCatalog || SupportsActionSpecificClean(m_selectedOperationId.toStdString()));

		if (OperationNeedsContent(m_selectedOperationId) && m_contentModel.ContentId().isEmpty())
		{
			const QString reason = "Workspace content is unavailable. Confirm this is a complete Sparkle workspace, then regenerate build files if needed.";
			ApplyActionAvailability(*m_runButton, false, reason);
			ApplyActionAvailability(*m_cleanButton, false, "Workspace content is unavailable for this clean action.");
			return;
		}

		const QVector<LauncherCleanTarget> cleanTargets = SupportsActionSpecificClean(m_selectedOperationId.toStdString())
		    ? BuildActionSpecificCleanTargets(
		          BuildActionCleanTargetRequest(m_workspaceRoot, m_contentModel, m_settings, std::filesystem::path(QCoreApplication::applicationFilePath().toStdString()), m_selectedOperationId))
		    : QVector<LauncherCleanTarget>();

		const bool bCleanWorkspaceSelected = m_selectedOperationId == "workspace.clean";
		const QString cleanScopeError = bCleanWorkspaceSelected ? CleanScopeSelectionError(m_settings.CleanScope()) : QString();
		if (!cleanScopeError.isEmpty())
		{
			ApplyActionAvailability(*m_runButton, false, cleanScopeError);
			return;
		}
		if (bIsLevelCatalog)
		{
			m_sceneCatalog->UpdateActionAvailability(*m_cleanButton, *m_runButton, !cleanTargets.isEmpty());
			return;
		}
		const bool bCanClean = !cleanTargets.isEmpty();

		ApplyActionAvailability(
		    *m_cleanButton,
		    bCanClean,
		    bCanClean ? "Clean only the generated outputs tied to " + DisplayNameForOperation(m_selectedOperationId) + "." : "Clean is not available for this workflow.");

		UpdateWorkflowRunAvailability();
	}

	void LauncherMainWindow::UpdateWorkflowRunAvailability()
	{
		if (FindBuildWorkspaceOperationDefinition(m_selectedOperationId.toStdString()).has_value())
		{
			const BuildWorkspaceOperationRequest request = BuildWorkspacePlanRequest(m_workspaceRoot, m_contentModel, m_settings);
			const BuildWorkspaceOperationPlan plan = PlanBuildWorkspaceOperation(m_selectedOperationId.toStdString(), request);
			const QString reason = plan.bCanRun ? "Run " + DisplayNameForOperation(m_selectedOperationId) + ". Existing runs keep going." : FirstBlockingReadinessMessage(plan.ReadinessMessages);
			ApplyActionAvailability(*m_runButton, plan.bCanRun, reason);
			return;
		}

		if (FindCookOperationDefinition(m_selectedOperationId.toStdString()).has_value())
		{
			const LauncherOperationRequest operationRequest = BuildLauncherOperationRequest(m_workspaceRoot, m_contentModel, m_settings, m_selectedOperationId);
			const CookOperationPlan plan = PlanCookOperation(m_selectedOperationId.toStdString(), LauncherOperationRequestMapping::Cook(operationRequest));

			const QString reason = plan.bCanRun
			    ? "Run " + DisplayNameForOperation(m_selectedOperationId) + ". Existing runs keep going."
			    : (plan.ReadinessMessages.empty() ? QStringLiteral("This cooking workflow is currently blocked.") : QString::fromStdString(plan.ReadinessMessages.back()));

			ApplyActionAvailability(*m_runButton, plan.bCanRun, reason);
			return;
		}

		const QString title = DisplayNameForOperation(m_selectedOperationId);
		const QString actionDescription = "Run " + title + ". Existing runs keep going.";
		ApplyActionAvailability(*m_runButton, true, actionDescription);
	}

	void LauncherMainWindow::SetSelectedOperation(const QString& operationId)
	{
		m_selectedOperationId = operationId;
		SetControlsEnabled(true);
		if (m_actionMetaPanel != nullptr)
		{
			m_actionMetaPanel->setVisible(true);
		}
		if (m_runButton != nullptr)
		{
			const bool bDestructive = operationId == "workspace.clean";
			LauncherIcon actionIcon = LauncherIcon::Run;
			if (operationId == LauncherScenesOperationId() || operationId.startsWith("workspace.sync"))
			{
				actionIcon = LauncherIcon::Sync;
			}
			else if (operationId.startsWith("workspace.build") || operationId == "launcher.build.self")
			{
				actionIcon = LauncherIcon::Build;
			}
			else if (operationId.startsWith("cook."))
			{
				actionIcon = LauncherIcon::Cook;
			}
			else if (bDestructive)
			{
				actionIcon = LauncherIcon::Clean;
			}
			m_runButton->setProperty("ActionTone", bDestructive ? "destructive" : "primary");
			m_runButton->setIcon(m_icons.Icon(actionIcon, QColor(bDestructive ? "#ffffff" : "#071006")));
			m_runButton->setText(operationId == LauncherScenesOperationId() ? "Sync All" : operationId == "workspace.clean" ? "Clean" : PrimaryActionLabelForOperationId(operationId));

			m_runButton->setAccessibleName(
			    operationId == LauncherScenesOperationId() ? "Sync all available levels"
			        : operationId == "workspace.clean"     ? "Clean generated repository state"
			                                               : "Run selected workflow");

			m_runButton->style()->unpolish(m_runButton);
			m_runButton->style()->polish(m_runButton);
		}
		if (m_cleanButton != nullptr)
		{
			const bool bCleanAllLevels = operationId == LauncherScenesOperationId();
			m_cleanButton->setProperty("ActionTone", "destructive");
			m_cleanButton->setText(bCleanAllLevels ? "Clean All" : "Clean");
			m_cleanButton->setAccessibleName(operationId == LauncherScenesOperationId() ? "Clean all selected levels" : "Clean selected workflow outputs");
			m_cleanButton->style()->unpolish(m_cleanButton);
			m_cleanButton->style()->polish(m_cleanButton);
		}
		if (m_optionsStack != nullptr)
		{
			EnsureOptionsPage(operationId);
			if (m_optionsPageByOperation.contains(operationId))
			{
				m_optionsStack->setCurrentIndex(m_optionsPageByOperation.value(operationId));
			}
		}

		if (m_workflowPanel != nullptr)
		{
			m_workflowPanel->SetSelectedOperation(operationId);
		}

		UpdateRunAvailability();
	}

	void LauncherMainWindow::ApplyVisualStyle()
	{
		ApplyLauncherVisualStyle(*this);
	}
}
