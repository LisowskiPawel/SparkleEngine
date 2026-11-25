#include "Gui/Shell/LauncherActivityPanel.h"

#include "Gui/Style/LauncherIconLibrary.h"
#include "Gui/Style/LauncherUiDesign.h"

#include <QtGui/QClipboard>
#include <QtGui/QColor>
#include <QtGui/QGuiApplication>
#include <QtGui/QKeySequence>
#include <QtGui/QTextCursor>
#include <QtGui/QTextDocument>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyle>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include <algorithm>
#include <utility>

namespace SparkleLauncher
{
	static constexpr int kMaxOperationOutputCharacters = 1000000;

	static int ProgressPercentage(quint64 completed, quint64 total)
	{
		if (total == 0)
		{
			return 0;
		}
		const quint64 boundedCompleted = std::min(completed, total);
		return static_cast<int>(static_cast<long double>(boundedCompleted) * 100.0L / static_cast<long double>(total));
	}

	static QString RunStateText(OperationStatus state)
	{
		switch (state)
		{
			case OperationStatus::Pending:
				return "Queued";
			case OperationStatus::Succeeded:
				return "Done";
			case OperationStatus::Skipped:
				return "Blocked";
			default:
				return QString::fromStdString(ToString(state));
		}
	}

	LauncherActivityPanel::LauncherActivityPanel(
	    const LauncherIconLibrary& icons,
	    const std::function<void(QWidget*)>& registerFocusable,
	    std::function<void(const QString&)> cancelRun,
	    QWidget* parent) :
	    QFrame(parent),
	    m_queuedIcon(icons.Icon(LauncherIcon::Queued, QColor(LauncherUi::Color::StateQueued))),
	    m_runningIcon(icons.Icon(LauncherIcon::Running, QColor(LauncherUi::Color::StateRunning))),
	    m_doneIcon(icons.Icon(LauncherIcon::Done, QColor(LauncherUi::Color::StateSuccess))),
	    m_canceledIcon(icons.Icon(LauncherIcon::Failed, QColor(LauncherUi::Color::StateWarning))),
	    m_failedIcon(icons.Icon(LauncherIcon::Failed, QColor(LauncherUi::Color::StateDestructive)))
	{
		setObjectName("ActivityBottomPanel");
		setMinimumHeight(LauncherUi::Activity::CollapsedHeight);
		setMaximumHeight(LauncherUi::Activity::CollapsedHeight);

		QVBoxLayout* layout = new QVBoxLayout(this);
		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(0);

		QFrame* header = new QFrame(this);
		header->setObjectName("ActivityHeader");
		QHBoxLayout* headerLayout = new QHBoxLayout(header);
		headerLayout->setContentsMargins(LauncherUi::Activity::HeaderMargins);
		headerLayout->setSpacing(LauncherUi::Space::Small);

		QLabel* activityTitle = new QLabel("Activity", header);
		activityTitle->setObjectName("OutputPaneLabel");
		headerLayout->addWidget(activityTitle, 0);
		headerLayout->addStretch(1);

		m_toggleOutputButton = new QPushButton(QString::fromLatin1(LauncherUi::Activity::ExpandGlyph), header);
		m_toggleOutputButton->setObjectName("ActivityToggleButton");
		m_toggleOutputButton->setFixedSize(LauncherUi::Activity::ToggleButtonSize);
		m_toggleOutputButton->setEnabled(false);
		m_toggleOutputButton->setVisible(false);
		m_toggleOutputButton->setToolTip("Run a workflow to view its activity.");
		m_toggleOutputButton->setAccessibleName("Toggle Activity panel");
		m_toggleOutputButton->setAccessibleDescription("Shows or minimizes recent runs and raw process output.");
		registerFocusable(m_toggleOutputButton);
		connect(m_toggleOutputButton, &QPushButton::clicked, this, [this]() { ToggleExpanded(); });
		headerLayout->addWidget(m_toggleOutputButton, 0);
		layout->addWidget(header, 0);

		m_detailsPanel = new QFrame(this);
		m_detailsPanel->setObjectName("ActivityDetailsPanel");
		QHBoxLayout* activityLayout = new QHBoxLayout(m_detailsPanel);
		activityLayout->setContentsMargins(0, 0, 0, 0);
		activityLayout->setSpacing(0);

		QFrame* activityRail = new QFrame(m_detailsPanel);
		activityRail->setObjectName("ActivityRail");
		activityRail->setMinimumWidth(LauncherUi::Activity::ListWidth);
		QVBoxLayout* activityRailLayout = new QVBoxLayout(activityRail);
		activityRailLayout->setContentsMargins(LauncherUi::Activity::RailMargins);
		activityRailLayout->setSpacing(LauncherUi::Space::XSmall - 1);

		QLabel* activityHeader = new QLabel("Runs", activityRail);
		activityHeader->setObjectName("OutputPaneLabel");
		activityRailLayout->addWidget(activityHeader, 0);

		m_runList = new QListWidget(activityRail);
		m_runList->setObjectName("ActivityList");
		m_runList->setAccessibleName("Activity runs");
		m_runList->setAccessibleDescription("Recent runs. Select one to review its summary and output.");
		registerFocusable(m_runList);
		connect(m_runList, &QListWidget::currentItemChanged, this, [this](QListWidgetItem* current, QListWidgetItem*) { DisplaySelectedRunOutput(current); });
		activityRailLayout->addWidget(m_runList, 1);
		activityLayout->addWidget(activityRail, 0);

		QFrame* outputPane = new QFrame(m_detailsPanel);
		outputPane->setObjectName("OutputPane");
		QVBoxLayout* outputLayout = new QVBoxLayout(outputPane);
		outputLayout->setContentsMargins(LauncherUi::Activity::OutputMargins);
		outputLayout->setSpacing(LauncherUi::Space::XSmall - 1);

		QHBoxLayout* outputHeaderLayout = new QHBoxLayout();
		outputHeaderLayout->setContentsMargins(0, 0, 0, 0);
		outputHeaderLayout->setSpacing(LauncherUi::Space::Small - 2);
		QLabel* outputHeader = new QLabel("Log", outputPane);
		outputHeader->setObjectName("OutputPaneLabel");
		outputHeaderLayout->addWidget(outputHeader, 0);
		outputHeaderLayout->addStretch(1);

		m_cancelRunButton = new QPushButton("Cancel run", outputPane);
		m_cancelRunButton->setObjectName("SecondaryButton");
		m_cancelRunButton->setAccessibleName("Cancel selected run");
		m_cancelRunButton->setVisible(false);
		registerFocusable(m_cancelRunButton);
		connect(
		    m_cancelRunButton,
		    &QPushButton::clicked,
		    this,
		    [this, cancelRun = std::move(cancelRun)]()
		    {
			    const QListWidgetItem* item = m_runList->currentItem();
			    if (item != nullptr)
			    {
				    cancelRun(item->data(Qt::UserRole).toString());
			    }
		    });
		outputHeaderLayout->addWidget(m_cancelRunButton);

		m_copyOutputButton = new QPushButton("Copy output", outputPane);
		m_copyOutputButton->setObjectName("SecondaryButton");
		m_copyOutputButton->setIcon(icons.Icon(LauncherIcon::Copy, QColor(LauncherUi::Color::StateQueued)));
		m_copyOutputButton->setIconSize(QSize(LauncherUi::Icon::DefaultSize, LauncherUi::Icon::DefaultSize));
		m_copyOutputButton->setEnabled(false);
		m_copyOutputButton->setVisible(false);
		m_copyOutputButton->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C));
		m_copyOutputButton->setToolTip("Select a run to copy its output. Shortcut: Ctrl+Shift+C.");
		m_copyOutputButton->setAccessibleName("Copy selected run output");
		m_copyOutputButton->setAccessibleDescription("Copies output for the selected run.");
		registerFocusable(m_copyOutputButton);
		connect(m_copyOutputButton, &QPushButton::clicked, this, [this]() { CopySelectedRunOutput(); });
		outputHeaderLayout->addWidget(m_copyOutputButton, 0);
		outputLayout->addLayout(outputHeaderLayout);

		m_selectedRunSummary = new QLabel("Select a run to view output.", outputPane);
		m_selectedRunSummary->setObjectName("ActivitySummary");
		m_selectedRunSummary->setAccessibleName("Selected activity summary");
		m_selectedRunSummary->setWordWrap(true);
		outputLayout->addWidget(m_selectedRunSummary);

		m_progressBar = new QProgressBar(outputPane);
		m_progressBar->setObjectName("ActivityProgress");
		m_progressBar->setAccessibleName("Selected activity progress");
		m_progressBar->setTextVisible(true);
		m_progressBar->setVisible(false);
		outputLayout->addWidget(m_progressBar);

		m_operationOutput = new QTextEdit(outputPane);
		m_operationOutput->setObjectName("OperationOutput");
		m_operationOutput->setReadOnly(true);
		m_noticeDocument = new QTextDocument(this);
		m_noticeDocument->setUndoRedoEnabled(false);
		m_operationOutput->setToolTip("Select a run to view its output.");
		m_operationOutput->setAccessibleName("Selected run output");
		m_operationOutput->setAccessibleDescription("Read-only output for the selected run.");
		m_operationOutput->setMinimumHeight(LauncherUi::OperationOutput::MinHeight);
		m_operationOutput->setMaximumHeight(LauncherUi::OperationOutput::MaxHeight);
		registerFocusable(m_operationOutput);
		outputLayout->addWidget(m_operationOutput);
		activityLayout->addWidget(outputPane, 1);
		layout->addWidget(m_detailsPanel, 1);

		SetExpanded(false);
	}

	void LauncherActivityPanel::ShowMessage(const QString& message)
	{
		m_noticeDocument->setDefaultFont(m_operationOutput->font());
		m_noticeDocument->setPlainText(message);
		m_operationOutput->setDocument(m_noticeDocument);
	}

	void LauncherActivityPanel::RegisterRun(const QString& runId, const QString& title)
	{
		QListWidgetItem* item = new QListWidgetItem(m_runList);
		item->setData(Qt::UserRole, runId);
		item->setSizeHint(QSize(0, LauncherUi::Activity::HistoryRowHeight));
		QTextDocument* output = new QTextDocument(this);
		output->setDefaultFont(m_operationOutput->font());
		output->setUndoRedoEnabled(false);
		output->setPlainText(title + " queued.\n");
		m_runs.insert(runId, {item, OperationStatus::Pending, title, output});
		SetRunState(runId, OperationStatus::Pending, title);
		m_runList->setCurrentItem(item);
		SetExpanded(m_bExpanded);
	}

	void LauncherActivityPanel::DisplayOperationStarted(const QString& runId, const QString& title)
	{
		const auto run = m_runs.constFind(runId);
		const QString effectiveTitle = run == m_runs.constEnd() ? title : run->Title;
		SetRunState(runId, OperationStatus::Running, effectiveTitle);
		AppendRunOutput(runId, effectiveTitle + " started.\n");
		ShowRunOutput(runId);
		SetExpanded(true);
	}

	void LauncherActivityPanel::AppendOperationOutput(const QString& runId, const QString& outputText)
	{
		AppendRunOutput(runId, outputText);
		const QListWidgetItem* item = m_runList->currentItem();
		if (item != nullptr && item->data(Qt::UserRole).toString() == runId)
		{
			ShowRunOutput(runId);
		}
	}

	void LauncherActivityPanel::UpdateOperationProgress(const QString& runId, const QString& phase, quint64 completed, quint64 total)
	{
		auto run = m_runs.find(runId);
		if (run == m_runs.end() || run->State != OperationStatus::Running)
		{
			return;
		}

		run->bHasProgress = true;
		run->ProgressPhase = phase;
		run->ProgressCompleted = completed;
		run->ProgressTotal = total;
		UpdateRunItem(*run);
		if (run->Item == m_runList->currentItem())
		{
			ShowRunOutput(runId);
		}
	}

	void LauncherActivityPanel::DisplayOperationFinished(const QString& runId, const OperationRecord& result)
	{
		auto run = m_runs.find(runId);
		if (run == m_runs.end())
		{
			return;
		}

		SetRunState(runId, result.Status, run->Title);
		const QString statusText = QString::fromStdString(ToString(result.Status));
		if (result.Status == OperationStatus::Succeeded)
		{
			AppendRunOutput(runId, "\nResult: " + statusText + "\n");
			if (!result.LogPath.empty())
			{
				AppendRunOutput(runId, "Log: " + QString::fromStdString(result.LogPath.string()) + "\n");
			}
		}
		else
		{
			QString diagnosis = "Result: " + statusText + "\n";
			const OperationFailure emptyFailure;
			const OperationFailure& failure = result.Failure ? *result.Failure : emptyFailure;
			diagnosis += "Problem type: " + (result.Failure ? QString::fromStdString(ToString(failure.Kind)) : QString()) + "\n";
			diagnosis += "What failed: " + QString::fromStdString(failure.Summary) + "\n";
			diagnosis += "Next action: " + QString::fromStdString(failure.ExpectedAction) + "\n";
			if (result.ExitCode.has_value())
			{
				diagnosis += QStringLiteral("Exit code: %1\n").arg(*result.ExitCode);
			}
			if (!result.LogPath.empty())
			{
				diagnosis += "Log: " + QString::fromStdString(result.LogPath.string()) + "\n";
			}
			QTextCursor cursor(run->Output);
			cursor.insertText(diagnosis + "\nProcess output:\n");
		}

		ShowRunOutput(runId);
		SetExpanded(true);
	}

	void LauncherActivityPanel::AppendRunOutput(const QString& runId, const QString& text)
	{
		auto run = m_runs.find(runId);
		if (run == m_runs.end() || text.isEmpty())
		{
			return;
		}

		const bool bSkipLineFeed = run->bOutputEndsWithCarriageReturn && text.startsWith('\n');
		run->bOutputEndsWithCarriageReturn = text.endsWith('\r');
		QTextCursor cursor(run->Output);
		cursor.movePosition(QTextCursor::End);
		cursor.insertText(bSkipLineFeed ? text.sliced(1) : text);
		const int overflowCharacters = run->Output->characterCount() - 1 - kMaxOperationOutputCharacters;
		if (overflowCharacters > 0)
		{
			cursor.setPosition(0);
			cursor.setPosition(overflowCharacters, QTextCursor::KeepAnchor);
			cursor.removeSelectedText();
		}
	}

	void LauncherActivityPanel::UpdateRunItem(const RunRecord& run)
	{
		QString stateText = RunStateText(run.State);
		if (run.State == OperationStatus::Running && run.ProgressTotal != 0)
		{
			stateText += QStringLiteral(" \u00b7 %1%").arg(ProgressPercentage(run.ProgressCompleted, run.ProgressTotal));
		}
		const QString caption = stateText + ": " + run.Title;
		run.Item->setText(caption);
		run.Item->setIcon(IconForState(run.State));
		run.Item->setData(Qt::UserRole + 1, stateText);
		run.Item->setData(Qt::AccessibleTextRole, caption);
		run.Item->setData(Qt::AccessibleDescriptionRole, "Launcher activity run " + RunStateText(run.State).toLower());
		run.Item->setToolTip(caption);
	}

	void LauncherActivityPanel::ShowRunOutput(const QString& runId)
	{
		const auto run = m_runs.constFind(runId);
		if (run == m_runs.constEnd())
		{
			return;
		}

		if (m_runList->currentItem() != run->Item)
		{
			m_runList->setCurrentItem(run->Item);
			return;
		}

		const OperationStatus state = run->State;
		m_cancelRunButton->setVisible(state == OperationStatus::Pending || state == OperationStatus::Running);
		const QString stateText = RunStateText(state);
		const QString stateName = stateText.toLower();
		QString summary = stateText + ": " + run->Title + ". ";
		if (state == OperationStatus::Running)
		{
			if (run->bHasProgress)
			{
				summary += run->ProgressPhase;
				if (run->ProgressTotal != 0)
				{
					summary += QStringLiteral(" \u2014 %1/%2 (%3%)")
					               .arg(static_cast<qulonglong>(run->ProgressCompleted))
					               .arg(static_cast<qulonglong>(run->ProgressTotal))
					               .arg(ProgressPercentage(run->ProgressCompleted, run->ProgressTotal));
				}
				summary += ".";
			}
			else
			{
				summary += "Waiting for progress from the active tool.";
			}
		}
		else if (state == OperationStatus::Pending)
		{
			summary += "Waiting to start.";
		}
		else if (state == OperationStatus::Succeeded)
		{
			summary += "Output is available below.";
		}
		else if (state == OperationStatus::Canceled)
		{
			summary += "Run it again when ready.";
		}
		else
		{
			summary += "Follow the next action below.";
		}
		m_selectedRunSummary->setText(summary);
		m_selectedRunSummary->setProperty("RunState", stateName);
		m_selectedRunSummary->style()->unpolish(m_selectedRunSummary);
		m_selectedRunSummary->style()->polish(m_selectedRunSummary);

		const bool bShowProgress = state == OperationStatus::Running && run->bHasProgress;
		m_progressBar->setVisible(bShowProgress);
		if (bShowProgress)
		{
			if (run->ProgressTotal == 0)
			{
				m_progressBar->setRange(0, 0);
				m_progressBar->setFormat(run->ProgressPhase);
			}
			else
			{
				m_progressBar->setRange(0, 100);
				m_progressBar->setValue(ProgressPercentage(run->ProgressCompleted, run->ProgressTotal));
				m_progressBar->setFormat(run->ProgressPhase + QStringLiteral(" — %p%"));
			}
			m_progressBar->setAccessibleDescription(m_selectedRunSummary->text());
		}

		const bool bCompactOutput = state == OperationStatus::Succeeded;
		m_operationOutput->setMinimumHeight(bCompactOutput ? LauncherUi::OperationOutput::MinHeight : LauncherUi::OperationOutput::ProminentMinHeight);
		m_operationOutput->setMaximumHeight(bCompactOutput ? LauncherUi::OperationOutput::CompactMaxHeight : LauncherUi::OperationOutput::MaxHeight);
		if (m_operationOutput->document() != run->Output)
		{
			m_operationOutput->setDocument(run->Output);
		}
		m_operationOutput->moveCursor(QTextCursor::End);

		const bool bCanCopyOutput = m_bExpanded && !m_operationOutput->document()->isEmpty();
		m_copyOutputButton->setVisible(!runId.isEmpty());
		m_copyOutputButton->setEnabled(bCanCopyOutput);
		m_copyOutputButton->setToolTip(bCanCopyOutput ? "Copy output for the selected run. Shortcut: Ctrl+Shift+C." : "Select a run to copy its output. Shortcut: Ctrl+Shift+C.");
	}

	bool LauncherActivityPanel::HasActiveRun() const
	{
		return std::any_of(m_runs.cbegin(), m_runs.cend(), [](const RunRecord& run) { return run.State == OperationStatus::Pending || run.State == OperationStatus::Running; });
	}

	QIcon LauncherActivityPanel::IconForState(OperationStatus state) const
	{
		switch (state)
		{
			case OperationStatus::Pending:
				return m_queuedIcon;
			case OperationStatus::Running:
				return m_runningIcon;
			case OperationStatus::Succeeded:
				return m_doneIcon;
			case OperationStatus::Skipped:
				return m_canceledIcon;
			case OperationStatus::Canceled:
				return m_canceledIcon;
			case OperationStatus::Failed:
				return m_failedIcon;
		}

		return {};
	}

	void LauncherActivityPanel::DisplaySelectedRunOutput(QListWidgetItem* currentItem)
	{
		if (currentItem == nullptr)
		{
			return;
		}

		const QString runId = currentItem->data(Qt::UserRole).toString();
		ShowRunOutput(runId);
		const auto run = m_runs.constFind(runId);
		const OperationStatus state = run == m_runs.constEnd() ? OperationStatus::Succeeded : run->State;
		if (state == OperationStatus::Running || state == OperationStatus::Skipped || state == OperationStatus::Canceled || state == OperationStatus::Failed)
		{
			SetExpanded(true);
		}
	}

	void LauncherActivityPanel::CopySelectedRunOutput()
	{
		QGuiApplication::clipboard()->setText(m_operationOutput->toPlainText());
	}

	void LauncherActivityPanel::ToggleExpanded()
	{
		if (m_runs.isEmpty())
		{
			return;
		}

		SetExpanded(!m_bExpanded);
	}

	void LauncherActivityPanel::SetRunState(const QString& runId, OperationStatus state, const QString& title)
	{
		auto run = m_runs.find(runId);
		if (run == m_runs.end() || run->Item == nullptr)
		{
			return;
		}

		run->State = state;
		run->Title = title;

		UpdateRunItem(*run);
	}

	void LauncherActivityPanel::SetExpanded(bool bExpanded)
	{
		const bool bHasRuns = !m_runs.isEmpty();
		bExpanded = bExpanded && bHasRuns;
		m_bExpanded = bExpanded;
		setMinimumHeight(bExpanded ? LauncherUi::Activity::ExpandedHeight : LauncherUi::Activity::CollapsedHeight);
		setMaximumHeight(bExpanded ? LauncherUi::Activity::ExpandedHeight : LauncherUi::Activity::CollapsedHeight);
		m_detailsPanel->setVisible(bExpanded);
		m_operationOutput->setVisible(bExpanded);

		m_toggleOutputButton->setEnabled(bHasRuns);
		m_toggleOutputButton->setVisible(bHasRuns);
		m_toggleOutputButton->setText(QString::fromLatin1(bExpanded ? LauncherUi::Activity::CollapseGlyph : LauncherUi::Activity::ExpandGlyph));
		m_toggleOutputButton->setToolTip(!bHasRuns ? "Run a workflow to view its activity." : bExpanded ? "Minimize recent runs and raw process output." : "Show recent runs and raw process output.");
		m_toggleOutputButton->setAccessibleDescription(m_toggleOutputButton->toolTip());

		const bool bCanCopyOutput = bExpanded && !m_operationOutput->document()->isEmpty();
		m_copyOutputButton->setVisible(bExpanded && m_runList->currentItem() != nullptr);
		m_copyOutputButton->setEnabled(bCanCopyOutput);
	}
}
