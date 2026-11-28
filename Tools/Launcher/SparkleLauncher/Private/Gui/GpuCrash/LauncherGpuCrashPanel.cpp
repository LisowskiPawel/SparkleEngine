#include "Gui/GpuCrash/LauncherGpuCrashPanel.h"
#include "Gui/App/LauncherSettings.h"
#include "Gui/Style/LauncherUiDesign.h"

#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
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
	LauncherGpuCrashPanel::LauncherGpuCrashPanel(bool bHasBuildSources, LauncherSettings& settings, QWidget* parent) :
	    QWidget(parent),
	    m_settings(settings)
	{
		setObjectName("FooterContextItem");
		auto* layout = new QVBoxLayout(this);
		layout->setContentsMargins(LauncherUi::FooterContext::FieldMargins);
		layout->setSpacing(LauncherUi::FooterContext::FieldSpacing);
		auto* heading = new QHBoxLayout();
		heading->setSpacing(LauncherUi::Space::XSmall);
		auto* label = new QLabel("GPU Crash Debugging", this);
		label->setObjectName("FooterFieldLabel");
		heading->addWidget(label);
		heading->addStretch();
		m_tools = new QToolButton(this);
		m_tools->setText("...");
		m_tools->setAutoRaise(true);
		m_tools->setFixedSize(18, 16);
		m_tools->setAccessibleName("GPU crash tools");
		m_tools->setToolTip("Configure shader export and launch a debugger.");
		m_tools->setEnabled(bHasBuildSources);
		heading->addWidget(m_tools);
		layout->addLayout(heading);
		connect(m_tools, &QToolButton::clicked, this, &LauncherGpuCrashPanel::ConfigureGpuCrashTools);

		m_enabled = new QCheckBox("Enabled", this);
		m_enabled->setFixedHeight(LauncherUi::ContextSelector::ComboHeight);
		m_enabled->setAccessibleName("GPU crash debugging");
		m_enabled->setToolTip("Use NVIDIA Aftermath with prepared diagnostic build and shader export. GPU Capture must be None.");
		layout->addWidget(m_enabled);
		label->setBuddy(m_enabled);
		const auto refresh = [this, bHasBuildSources]()
		{
			const bool bActive = m_settings.GpuCrashMode() != RhiGpuCrashMode::Off;
			m_enabled->setChecked(bActive);
			m_enabled->setEnabled(bHasBuildSources || bActive);
		};
		refresh();
		connect(&m_settings, &LauncherSettings::SettingsChanged, this, refresh);
		connect(m_enabled, &QCheckBox::clicked, this, [this](bool bActive) { m_settings.SetGpuCrashMode(bActive ? "aftermath" : "off"); });
		if (!bHasBuildSources)
		{
			m_enabled->setToolTip("GPU crash debugging requires a source workspace and prepared diagnostic build.");
		}
	}

	void LauncherGpuCrashPanel::ConfigureGpuCrashTools()
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
		auto* debugger = new QLineEdit(m_settings.GpuCrashInspectorPath(), &dialog);
		debugger->setPlaceholderText("Debugger executable");
		debugger->setToolTip("Choose an installed Nsight Graphics executable.");
		form->addRow("Debugger", debugger);
		connect(debugger, &QLineEdit::textChanged, &m_settings, &LauncherSettings::SetGpuCrashInspectorPath);
		layout->addLayout(form);
		auto* open = new QPushButton("Open debugger", &dialog);
		open->setObjectName("CommandSecondaryButton");
		layout->addWidget(open);
		connect(open, &QPushButton::clicked, this, &LauncherGpuCrashPanel::LaunchConfiguredGpuDebugger);
		auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
		connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
		layout->addWidget(buttons);
		dialog.exec();
	}

	void LauncherGpuCrashPanel::LaunchConfiguredGpuDebugger()
	{
		const QFileInfo tool(m_settings.GpuCrashInspectorPath());
		const bool bExecutableFile = tool.isAbsolute() && tool.isFile();
		const bool bValidTool = bExecutableFile && tool.suffix().compare("exe", Qt::CaseInsensitive) == 0;
		const bool bStarted = bValidTool && QProcess::startDetached(tool.absoluteFilePath(), {}, tool.absolutePath());
		if (!bStarted)
		{
			QMessageBox::information(this, "Debugger unavailable", "Choose an installed debugger executable that can be started.");
		}
	}

	std::array<QWidget*, 2> LauncherGpuCrashPanel::GetFocusableWidgets() const noexcept
	{
		return {m_enabled, m_tools};
	}
}
