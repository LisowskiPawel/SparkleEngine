#include "LauncherMainWindow.h"

#include "LauncherSettings.h"
#include "Core/Public/Paths/ProductPaths.h"

#include <QtCore/QProcess>
#include <QtCore/QUrl>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

#include <algorithm>

namespace SparkleLauncher
{
	void LauncherMainWindow::AddGpuCrashOptions(QVBoxLayout& layout)
	{
		if (!Filesystem::IsSourceWorkspace(m_repositoryRoot))
		{
			return;
		}

		auto* group = AddOptionGroup(layout, "GPU crash debugging", "Uses the diagnostic Editor/Game build. Captures and source mapping are qualified separately.");
		auto* mode = CreateValueCombo({{"Off", "off"}, {"NVIDIA Aftermath", "aftermath"}}, m_settings.GpuCrashMode() == RhiGpuCrashMode::Off ? "off" : "aftermath", &LauncherSettings::SetGpuCrashMode);
		AddOptionField(*group, "Mode", mode);

		auto* shaderExport = CreateBoundLineEdit(
		    m_settings.ShaderExportPath(),
		    "Exact shader-export.json",
		    "Paste the absolute manifest path returned by the diagnostic shader cook.",
		    &LauncherSettings::SetShaderExportPath);

		AddOptionField(*group, "Shader export", shaderExport);

		auto* inspector = CreateBoundLineEdit(
		    m_settings.GpuCrashInspectorPath(),
		    "Inspector executable",
		    "Choose an installed Nsight Graphics executable. No installation or association is required by Sparkle.",
		    &LauncherSettings::SetGpuCrashInspectorPath);

		AddOptionField(*group, "Inspector", inspector);
		auto* status = new QLabel("No diagnostic child has confirmed native readiness. Source mapping: unqualified.", this);
		status->setWordWrap(true);
		group->addWidget(status);
		m_gpuCrashStatusLabels.erase(std::remove_if(m_gpuCrashStatusLabels.begin(), m_gpuCrashStatusLabels.end(), [](const auto& label) { return label.isNull(); }), m_gpuCrashStatusLabels.end());
		m_gpuCrashStatusLabels.push_back(status);
		RefreshGpuCrashIncident();

		auto* refresh = new QPushButton("Refresh collection state", this);
		refresh->setObjectName("CommandSecondaryButton");
		RegisterFocusable(refresh);
		group->addWidget(refresh);
		connect(refresh, &QPushButton::clicked, this, [this]() { RefreshGpuCrashIncident(); });

		auto* folder = new QPushButton("Open incident folder", this);
		folder->setObjectName("CommandSecondaryButton");
		RegisterFocusable(folder);
		group->addWidget(folder);

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

		auto* open = new QPushButton("Verify and open inspector", this);
		open->setObjectName("CommandSecondaryButton");
		RegisterFocusable(open);
		group->addWidget(open);

		connect(open, &QPushButton::clicked, this, [this, open]() { OpenGpuCrashInspector(*open); });

		auto* privacy = new QLabel(
		    "Private local files only. ExportDiagnostics.ps1 previews sanitized metadata; raw dump, NVIDIA debug data, source and symbols require explicit category switches and preview consent. No "
		    "upload or support service.",
		    this);

		privacy->setWordWrap(true);
		group->addWidget(privacy);
	}

	void LauncherMainWindow::OpenGpuCrashInspector(QPushButton& button)
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
		const QString script = QString::fromStdWString((m_repositoryRoot / "CMake/Diagnostics/OpenGpuCrash.ps1").wstring());

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

	void LauncherMainWindow::RefreshGpuCrashIncident()
	{
		if (!m_gpuCrashIncident || m_gpuCrashQueryPending)
		{
			return;
		}

		m_gpuCrashQueryPending = true;
		const QPointer<LauncherMainWindow> window(this);
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

	void LauncherMainWindow::DisplayGpuCrashState(const QString& state)
	{
		for (auto label = m_gpuCrashStatusLabels.begin(); label != m_gpuCrashStatusLabels.end();)
		{
			if (!*label)
			{
				label = m_gpuCrashStatusLabels.erase(label);
				continue;
			}
			(*label)->setText("Collection (last recorded): " + state + ". Source mapping: unqualified. Refresh after the child exits.");
			++label;
		}
	}

}
