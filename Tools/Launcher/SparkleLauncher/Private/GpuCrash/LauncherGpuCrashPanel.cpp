#include "LauncherGpuCrashPanel.h"
#include "LauncherBackend.h"

#include <utility>

#include "LauncherSettings.h"
#include "LauncherProductPaths.h"
#include "LauncherUiDesign.h"

#include <QtCore/QProcess>
#include <QtCore/QUrl>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>

namespace SparkleLauncher
{
	LauncherGpuCrashPanel::LauncherGpuCrashPanel(std::filesystem::path workspaceRoot, LauncherSettings& settings, LauncherBackend& backend, QWidget* parent) :
	    QWidget(parent),
	    m_workspaceRoot(std::move(workspaceRoot)),
	    m_settings(settings),
	    m_backend(backend)
	{
		auto* item = this;
		item->setObjectName("FooterContextItem");
		auto* layout = new QVBoxLayout(item);
		layout->setContentsMargins(LauncherUi::FooterContext::FieldMargins);
		layout->setSpacing(LauncherUi::FooterContext::FieldSpacing);
		auto* heading = new QHBoxLayout();
		heading->setSpacing(LauncherUi::Space::XSmall);
		auto* label = new QLabel("GPU Crash Debugging", item);
		label->setObjectName("FooterFieldLabel");
		heading->addWidget(label);
		heading->addStretch();

		auto* tools = new QToolButton(item);
		tools->setText("...");
		tools->setAutoRaise(true);
		tools->setFixedSize(18, 16);
		tools->setAccessibleName("GPU crash tools");
		tools->setToolTip("Shader export, incident files and inspector.");
		heading->addWidget(tools);
		layout->addLayout(heading);
		connect(tools, &QToolButton::clicked, this, &LauncherGpuCrashPanel::OpenGpuCrashTools);

		auto* enabled = new QCheckBox("Enabled", item);
		enabled->setFixedHeight(LauncherUi::ContextSelector::ComboHeight);
		enabled->setAccessibleName("GPU crash debugging");
		enabled->setToolTip("Use NVIDIA Aftermath with prepared diagnostic build and shader export. GPU Capture must be None.");
		layout->addWidget(enabled);
		label->setBuddy(enabled);

		const bool hasBuildSources = HasBuildSources(m_workspaceRoot);

		const auto refresh = [this, enabled, hasBuildSources]()
		{
			const bool active = m_settings.GpuCrashMode() != RhiGpuCrashMode::Off;
			enabled->setChecked(active);
			enabled->setEnabled(hasBuildSources || active);
		};

		refresh();
		connect(&m_settings, &LauncherSettings::SettingsChanged, item, refresh);
		connect(enabled, &QCheckBox::clicked, this, [this](bool active) { m_settings.SetGpuCrashMode(active ? "aftermath" : "off"); });

		tools->setEnabled(hasBuildSources);
		if (!hasBuildSources)
		{
			enabled->setToolTip("GPU crash debugging requires a source workspace and prepared diagnostic build.");
		}
		m_enabled = enabled;
		m_tools = tools;
	}

	void LauncherGpuCrashPanel::OpenGpuCrashTools()
	{
		QDialog dialog(this);
		dialog.setWindowTitle("GPU crash tools");
		dialog.setMinimumWidth(640);
		auto* layout = new QVBoxLayout(&dialog);
		auto* form = new QFormLayout();
		auto* shaderExport = new QLineEdit(m_settings.ShaderExportPath(), &dialog);
		shaderExport->setPlaceholderText("Exact shader-export.json");
		shaderExport->setToolTip("Paste the absolute manifest path returned by the diagnostic shader cook.");
		form->addRow("Shader export", shaderExport);
		connect(shaderExport, &QLineEdit::textChanged, &m_settings, &LauncherSettings::SetShaderExportPath);
		auto* inspector = new QLineEdit(m_settings.GpuCrashInspectorPath(), &dialog);
		inspector->setPlaceholderText("Inspector executable");
		inspector->setToolTip("Choose an installed Nsight Graphics executable.");
		form->addRow("Inspector", inspector);
		connect(inspector, &QLineEdit::textChanged, &m_settings, &LauncherSettings::SetGpuCrashInspectorPath);
		layout->addLayout(form);
		auto* status = new QLabel("No diagnostic child has confirmed native readiness. Source mapping: unqualified.", &dialog);
		status->setWordWrap(true);
		layout->addWidget(status);
		m_gpuCrashStatusLabel = status;
		RefreshGpuCrashIncident();

		auto* actions = new QHBoxLayout();
		auto* refresh = new QPushButton("Refresh collection state", &dialog);
		refresh->setObjectName("CommandSecondaryButton");
		actions->addWidget(refresh);
		connect(refresh, &QPushButton::clicked, this, [this]() { RefreshGpuCrashIncident(); });

		auto* folder = new QPushButton("Open incident folder", &dialog);
		folder->setObjectName("CommandSecondaryButton");
		folder->setEnabled(m_gpuCrashIncident.has_value());
		actions->addWidget(folder);

		connect(
		    folder,
		    &QPushButton::clicked,
		    this,
		    [this]()
		    {
			    if (m_gpuCrashIncident)
			    {
				    QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdWString(m_gpuCrashIncident->ManifestPath.parent_path().wstring())));
			    }
		    });

		auto* open = new QPushButton("Verify and open inspector", &dialog);
		open->setObjectName("CommandSecondaryButton");
		actions->addWidget(open);
		layout->addLayout(actions);

		connect(open, &QPushButton::clicked, this, [this, open]() { OpenGpuCrashInspector(*open); });

		auto* privacy = new QLabel(
		    "Private local files only. ExportDiagnostics.ps1 previews sanitized metadata; raw dump, NVIDIA debug data, source and symbols require explicit category switches and preview consent. No "
		    "upload or support service.",
		    &dialog);

		privacy->setWordWrap(true);
		layout->addWidget(privacy);
		auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
		connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
		layout->addWidget(buttons);
		dialog.exec();
	}

	void LauncherGpuCrashPanel::OpenGpuCrashInspector(QPushButton& button)
	{
		if (!m_gpuCrashIncident || m_settings.GpuCrashInspectorPath().isEmpty())
		{
			QMessageBox::information(this, "GPU crash inspection", "Keep the incident locally. Configure an inspector executable after a diagnostic child has produced a dump.");
			return;
		}

		auto* process = new QProcess(this);
		button.setEnabled(false);
		const QPointer<QPushButton> openButton(&button);

		connect(
		    process,
		    &QProcess::finished,
		    this,
		    [this, process, openButton](int exitCode, QProcess::ExitStatus)
		    {
			    if (openButton)
			    {
				    openButton->setEnabled(true);
			    }
			    const auto output = QString::fromUtf8(process->readAllStandardOutput()) + QString::fromUtf8(process->readAllStandardError());
			    QMessageBox::information(this, exitCode == 0 ? "Inspector search paths" : "Inspector unavailable", output);
			    process->deleteLater();
		    });

		connect(
		    process,
		    &QProcess::errorOccurred,
		    this,
		    [this, process, openButton](QProcess::ProcessError)
		    {
			    if (openButton)
			    {
				    openButton->setEnabled(true);
			    }
			    QMessageBox::information(this, "Inspector unavailable", "The inspection helper could not start. The incident is retained; use Open incident folder.");
			    process->deleteLater();
		    });

		const auto& incident = *m_gpuCrashIncident;
		const QString script = QString::fromStdWString((m_workspaceRoot / "CMake/Diagnostics/OpenGpuCrash.ps1").wstring());

		process->start(
		    "powershell.exe",
		    {"-NoProfile",
		        "-ExecutionPolicy",
		        "Bypass",
		        "-File",
		        script,
		        "-ManifestPath",
		        QString::fromStdWString(incident.ManifestPath.wstring()),
		        "-ExpectedSessionId",
		        QString::fromStdString(incident.SessionId),
		        "-ExpectedChildPid",
		        QString::number(incident.ChildPid),
		        "-InspectorPath",
		        m_settings.GpuCrashInspectorPath()});
	}

	void LauncherGpuCrashPanel::RefreshGpuCrashIncident()
	{
		if (!m_gpuCrashIncident || m_gpuCrashQueryPending)
		{
			return;
		}

		m_gpuCrashQueryPending = true;
		const QPointer<LauncherGpuCrashPanel> window(this);
		const auto sessionId = m_gpuCrashIncident->SessionId;

		const bool accepted = m_backend.ObserveGpuCrash(
		    *m_gpuCrashIncident,
		    [window, sessionId](const QString& state)
		    {
			    if (!window)
			    {
				    return;
			    }
			    window->m_gpuCrashQueryPending = false;
			    if (!window->m_gpuCrashIncident || window->m_gpuCrashIncident->SessionId != sessionId)
			    {
				    window->RefreshGpuCrashIncident();
				    return;
			    }
			    window->DisplayGpuCrashState(state);
		    });

		if (!accepted)
		{
			m_gpuCrashQueryPending = false;
			DisplayGpuCrashState("Observation unavailable: the operation lane rejected this query");
		}
	}

	void LauncherGpuCrashPanel::DisplayGpuCrashState(const QString& state)
	{
		if (m_gpuCrashStatusLabel)
		{
			m_gpuCrashStatusLabel->setText("Collection (last recorded): " + state + ". Source mapping: unqualified. Refresh after the child exits.");
		}
	}

	void LauncherGpuCrashPanel::SetIncident(GpuCrashIncident incident)
	{
		m_gpuCrashIncident = std::move(incident);
		RefreshGpuCrashIncident();
	}

	std::array<QWidget*, 2> LauncherGpuCrashPanel::GetFocusableWidgets() const noexcept
	{
		return {m_enabled, m_tools};
	}

}
