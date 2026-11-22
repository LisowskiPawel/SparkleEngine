#pragma once

#include "SparkleLauncher/OperationModel.h"

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtGui/QIcon>
#include <QtWidgets/QFrame>

#include <functional>

class QLabel;
class QListWidget;
class QListWidgetItem;
class QProgressBar;
class QPushButton;
class QTextEdit;
class QTextDocument;
class QWidget;

namespace SparkleLauncher
{
	class LauncherIconLibrary;
	struct LauncherOperationResult;

	class LauncherActivityPanel final : public QFrame
	{
	public:
		LauncherActivityPanel(const LauncherIconLibrary& icons, const std::function<void(QWidget*)>& registerFocusable, std::function<void(const QString&)> cancelRun, QWidget* parent = nullptr);

		void ShowMessage(const QString& message);
		void RegisterRun(const QString& runId, const QString& title);
		void DisplayOperationStarted(const QString& runId, const QString& title);
		void AppendOperationOutput(const QString& runId, const QString& outputText);
		void UpdateOperationProgress(const QString& runId, const QString& phase, quint64 completed, quint64 total);
		void DisplayOperationFinished(const QString& runId, const LauncherOperationResult& result);
		void ShowRunOutput(const QString& runId);
		bool HasActiveRun() const;

	private:
		struct RunRecord
		{
			QListWidgetItem* Item = nullptr;
			OperationStatus State = OperationStatus::Pending;
			QString Title;
			QTextDocument* Output = nullptr;
			bool bOutputEndsWithCarriageReturn = false;
			QString ProgressPhase;
			quint64 ProgressCompleted = 0;
			quint64 ProgressTotal = 0;
			bool bHasProgress = false;
		};

		QIcon IconForState(OperationStatus state) const;
		void DisplaySelectedRunOutput(QListWidgetItem* currentItem);
		void CopySelectedRunOutput();
		void ToggleExpanded();
		void SetRunState(const QString& runId, OperationStatus state, const QString& title);
		void AppendRunOutput(const QString& runId, const QString& text);
		void UpdateRunItem(const RunRecord& run);
		void SetExpanded(bool bExpanded);

		QFrame* m_detailsPanel = nullptr;
		QListWidget* m_runList = nullptr;
		QLabel* m_selectedRunSummary = nullptr;
		QProgressBar* m_progressBar = nullptr;
		QTextEdit* m_operationOutput = nullptr;
		QTextDocument* m_noticeDocument = nullptr;
		QPushButton* m_toggleOutputButton = nullptr;
		QPushButton* m_copyOutputButton = nullptr;
		QPushButton* m_cancelRunButton = nullptr;
		QHash<QString, RunRecord> m_runs;
		QIcon m_queuedIcon;
		QIcon m_runningIcon;
		QIcon m_doneIcon;
		QIcon m_canceledIcon;
		QIcon m_failedIcon;
		bool m_bExpanded = false;
	};
}
