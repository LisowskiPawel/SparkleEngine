#include <QtCore/QSignalBlocker>
#include "Gui/Widgets/LauncherOptionWidgets.h"
#include "GpuCrash/LauncherGpuCrashPanel.h"
#include "Gui/Shell/LauncherMainWindow.h"

#include "Gui/Widgets/LauncherLayoutWidgets.h"
#include "Gui/Models/LauncherContentModel.h"
#include "Gui/Models/LauncherContextUiModel.h"
#include "Gui/Models/LauncherExternalCaptureUiModel.h"
#include "Gui/Models/LauncherOperationRequestFactory.h"
#include "Gui/Widgets/LauncherSelectionWidgets.h"
#include "Gui/App/LauncherSettings.h"
#include "Gui/Style/LauncherUiDesign.h"
#include "Gui/Models/LauncherWorkflowCatalog.h"

#include "SparkleLauncher/BuildWorkspaceOperations.h"
#include "Workspace/LauncherProductPaths.h"

#include <QtGui/QColor>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSizePolicy>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>

namespace SparkleLauncher
{
	static constexpr int kLauncherIconSize = LauncherUi::Icon::DefaultSize;

	static void ApplyFooterContextComboMetrics(QComboBox& combo, int minWidth, int maxWidth)
	{
		combo.setMinimumWidth(minWidth);
		combo.setMaximumWidth(maxWidth);
		combo.setMinimumHeight(LauncherUi::ContextSelector::ComboHeight);
		combo.setMaximumHeight(LauncherUi::ContextSelector::ComboHeight);
	}

	static void PopulateBoundContextCombo(
	    QComboBox* combo,
	    const QVector<LauncherSelectionOption>& options,
	    const QString& currentValue,
	    LauncherSettings& settings,
	    void (LauncherSettings::*setter)(const QString&))
	{
		if (combo == nullptr)
		{
			return;
		}

		const QString effectiveValue = PopulateLauncherSelectionCombo(*combo, options, currentValue);
		if (!effectiveValue.isEmpty() && effectiveValue != currentValue)
		{
			(settings.*setter)(effectiveValue);
		}
	}

	QWidget* LauncherMainWindow::CreateOptionsPanel(QWidget* parent)
	{
		QFrame* panel = new QFrame(parent);
		panel->setObjectName("OptionsPanel");
		QVBoxLayout* layout = new QVBoxLayout(panel);
		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(0);

		m_optionsStack = new QStackedWidget(panel);
		m_optionsStack->setObjectName("OptionsStack");
		RebuildOptionsPages();
		layout->addWidget(m_optionsStack, 1);
		m_optionsStack->setVisible(false);

		m_actionMetaPanel = new QFrame(panel);
		m_actionMetaPanel->setObjectName("ActionMetaPanel");
		QHBoxLayout* actionMetaRowLayout = new QHBoxLayout(m_actionMetaPanel);
		actionMetaRowLayout->setContentsMargins(LauncherUi::ActionMeta::Margins);
		actionMetaRowLayout->setSpacing(LauncherUi::ActionMeta::Spacing);
		actionMetaRowLayout->addStretch(1);

		m_cleanButton = new QPushButton("Clean", panel);
		m_cleanButton->setObjectName("SecondaryButton");
		m_cleanButton->setFixedSize(LauncherUi::ActionMeta::SecondaryButtonWidth, LauncherUi::ActionMeta::ButtonHeight);
		m_cleanButton->setToolTip("Clean only the generated outputs tied to this action.");
		m_cleanButton->setEnabled(false);
		m_cleanButton->setAccessibleName("Clean selected workflow outputs");
		RegisterFocusable(m_cleanButton);
		connect(m_cleanButton, &QPushButton::clicked, this, &LauncherMainWindow::CleanSelectedOperation);
		actionMetaRowLayout->addWidget(m_cleanButton, 0, Qt::AlignRight | Qt::AlignVCenter);

		m_runButton = new QPushButton("Run", panel);
		m_runButton->setObjectName("PrimaryActionButton");
		m_runButton->setFixedSize(LauncherUi::ActionMeta::PrimaryButtonWidth, LauncherUi::ActionMeta::ButtonHeight);
		m_runButton->setIcon(m_icons.Icon(LauncherIcon::Run, QColor("#071006")));
		m_runButton->setIconSize(QSize(kLauncherIconSize, kLauncherIconSize));
		m_runButton->setToolTip("Run the selected workflow. Existing runs keep going.");
		m_runButton->setEnabled(false);
		m_runButton->setAccessibleName("Run selected workflow");
		RegisterFocusable(m_runButton);
		connect(m_runButton, &QPushButton::clicked, this, &LauncherMainWindow::RunSelectedOperation);
		actionMetaRowLayout->addWidget(m_runButton, 0, Qt::AlignRight | Qt::AlignVCenter);
		layout->addWidget(m_actionMetaPanel);
		return panel;
	}

	QWidget* LauncherMainWindow::CreateFooterContextPanel(QWidget* parent)
	{
		QFrame* panel = new QFrame(parent);
		panel->setObjectName("FooterContextPanel");
		QHBoxLayout* rowLayout = new QHBoxLayout(panel);
		rowLayout->setContentsMargins(LauncherUi::FooterContext::Margins);
		rowLayout->setSpacing(LauncherUi::FooterContext::ItemSpacing);
		rowLayout->addStretch(1);

		const auto addContextItem = [this, panel, rowLayout](const QString& labelText, QComboBox& combo, int minWidth, int maxWidth)
		{
			QWidget* item = new QWidget(panel);
			item->setObjectName("FooterContextItem");
			QVBoxLayout* itemLayout = new QVBoxLayout(item);
			itemLayout->setContentsMargins(LauncherUi::FooterContext::FieldMargins);
			itemLayout->setSpacing(LauncherUi::FooterContext::FieldSpacing);

			QLabel* label = CreateFieldLabel(labelText);
			label->setObjectName("FooterFieldLabel");
			combo.setObjectName("FooterContextCombo");
			ApplyFooterContextComboMetrics(combo, minWidth, maxWidth);
			label->setBuddy(&combo);
			itemLayout->addWidget(label, 0, Qt::AlignLeft | Qt::AlignBottom);
			itemLayout->addWidget(&combo, 0);
			rowLayout->addWidget(item, 0, Qt::AlignVCenter);
		};

		const auto addGroupDivider = [panel, rowLayout]()
		{
			QFrame* divider = new QFrame(panel);
			divider->setObjectName("FooterContextDivider");
			divider->setFixedSize(1, LauncherUi::FooterContext::DividerHeight);
			rowLayout->addWidget(divider, 0, Qt::AlignVCenter);
		};

		m_runModeCombo = CreateContextCombo(&LauncherSettings::SetRunMode);
		m_runModeCombo->setAccessibleName("Run Mode");
		m_runModeCombo->setToolTip("Open the selected scene in Editor or run it in Game.");
		addContextItem("Run Mode", *m_runModeCombo, LauncherUi::ContextSelector::RunModeComboMinWidth, LauncherUi::ContextSelector::RunModeComboMaxWidth);
		addGroupDivider();

		m_graphicsApiCombo = CreateContextCombo(&LauncherSettings::SetGraphicsApi);
		m_graphicsApiCombo->setAccessibleName("Graphics API");
		m_graphicsApiCombo->setToolTip("Graphics API used for the selected scene.");
		addContextItem("Graphics API", *m_graphicsApiCombo, LauncherUi::ContextSelector::GraphicsApiComboMinWidth, LauncherUi::ContextSelector::GraphicsApiComboMaxWidth);

		m_shaderBackendCombo = CreateContextCombo(&LauncherSettings::SetShaderBackend);
		m_shaderBackendCombo->setAccessibleName("Shader Compiler");
		m_shaderBackendCombo->setToolTip("DXC or Slang backend passed to shader cook operations.");
		addContextItem("Shader Compiler", *m_shaderBackendCombo, LauncherUi::ContextSelector::ShaderBackendComboMinWidth, LauncherUi::ContextSelector::ShaderBackendComboMaxWidth);

		m_captureProviderCombo = CreateContextCombo(&LauncherSettings::SetCaptureProvider);
		m_captureProviderCombo->setAccessibleName("GPU Capture");
		m_captureProviderCombo->setToolTip("Attach a capture tool when the Editor starts.");
		m_captureProviderCombo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
		addContextItem("GPU Capture", *m_captureProviderCombo, LauncherUi::ContextSelector::CaptureComboWidth, LauncherUi::ContextSelector::CaptureComboWidth);
		m_gpuCrashPanel = new LauncherGpuCrashPanel(m_workspaceRoot, m_settings, m_backend, panel);
		for (QWidget* widget : m_gpuCrashPanel->GetFocusableWidgets())
		{
			RegisterFocusable(widget);
		}
		rowLayout->addWidget(m_gpuCrashPanel, 0, Qt::AlignVCenter);
		addGroupDivider();

		m_buildConfigurationCombo = CreateContextCombo(&LauncherSettings::SetBuildConfiguration);
		m_buildConfigurationCombo->setAccessibleName("Build Configuration");
		m_buildConfigurationCombo->setToolTip("Global editor and game configuration. Only configurations backed by both product profiles are selectable.");
		addContextItem("Configuration", *m_buildConfigurationCombo, LauncherUi::ContextSelector::ConfigurationComboMinWidth, LauncherUi::ContextSelector::ConfigurationComboMaxWidth);

		m_workspaceCompilerCombo = CreateContextCombo(&LauncherSettings::SetWorkspaceCompiler);
		m_workspaceCompilerCombo->setAccessibleName("Compiler");

		m_workspaceCompilerCombo->setToolTip(
		    "Compiler configured by the launcher. Installed compilers are selectable; supported missing compilers remain visible for "
		    "setup.");

		addContextItem("Compiler", *m_workspaceCompilerCombo, LauncherUi::ContextSelector::CompilerComboMinWidth, LauncherUi::ContextSelector::CompilerComboMaxWidth);

		m_workspaceIdeCombo = CreateContextCombo(&LauncherSettings::SetWorkspaceIde);
		m_workspaceIdeCombo->setAccessibleName("IDE");
		m_workspaceIdeCombo->setToolTip("IDE used when generating workspace files. Detected IDEs are selectable; supported missing IDEs remain visible for setup.");
		addContextItem("IDE", *m_workspaceIdeCombo, LauncherUi::ContextSelector::IdeComboMinWidth, LauncherUi::ContextSelector::IdeComboMaxWidth);
		return panel;
	}

	QComboBox* LauncherMainWindow::CreateContextCombo(void (LauncherSettings::*setter)(const QString&))
	{
		QComboBox* combo = new QComboBox(this);
		RegisterFocusable(combo);

		connect(
		    combo,
		    static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
		    [combo, setter, this]()
		    {
			    const QString value = combo->currentData().toString();
			    if (!value.isEmpty())
			    {
				    (m_settings.*setter)(value);
			    }
		    });

		return combo;
	}

	void LauncherMainWindow::RefreshContextSelectors()
	{
		const bool bInstalled = !HasBuildSources(m_workspaceRoot);
		if (bInstalled)
		{
			m_settings.SetBuildConfiguration("shipping");
		}

		const BuildToolchainStatus toolchain = bInstalled ? BuildToolchainStatus{}
		                                                 : DetectBuildToolchain(m_workspaceRoot, ResolveSelectedWorkspaceIde(m_settings), ResolveSelectedWorkspaceCompiler(m_settings));

		const LauncherContextUiModel model = LauncherContextUiModel::Build(toolchain, m_workspaceRoot);
		PopulateBoundContextCombo(m_runModeCombo, model.RunModes, m_settings.RunMode(), m_settings, &LauncherSettings::SetRunMode);
		PopulateBoundContextCombo(m_graphicsApiCombo, model.GraphicsApis, m_settings.GraphicsApi(), m_settings, &LauncherSettings::SetGraphicsApi);
		const QString profile = m_settings.RunMode() == "editor" ? m_settings.EditorProfile() : m_settings.GameProfile();

		PopulateBoundContextCombo(
		    m_captureProviderCombo,
		    BuildExternalCaptureOptions(m_settings.GraphicsApi().toStdString(), profile.toStdString()),
		    QString::fromUtf8(ExternalCaptureProviderToString(m_settings.CaptureProvider()).data()),
		    m_settings,
		    &LauncherSettings::SetCaptureProvider);

		PopulateBoundContextCombo(m_shaderBackendCombo, model.ShaderBackends, m_settings.ShaderBackend(), m_settings, &LauncherSettings::SetShaderBackend);
		PopulateBoundContextCombo(m_buildConfigurationCombo, model.BuildConfigurations, m_settings.BuildConfiguration(), m_settings, &LauncherSettings::SetBuildConfiguration);
		PopulateBoundContextCombo(m_workspaceCompilerCombo, model.Compilers, m_settings.WorkspaceCompiler(), m_settings, &LauncherSettings::SetWorkspaceCompiler);
		PopulateBoundContextCombo(m_workspaceIdeCombo, model.Ides, m_settings.WorkspaceIde(), m_settings, &LauncherSettings::SetWorkspaceIde);
	}

	QWidget* LauncherMainWindow::CreateOptionsPage(const QString& operationId, QWidget* parent)
	{
		QScrollArea* scrollArea = new QScrollArea(parent);
		scrollArea->setObjectName("OptionsScrollArea");
		scrollArea->setWidgetResizable(true);
		scrollArea->setFrameShape(QFrame::NoFrame);
		scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

		QWidget* content = new QWidget(scrollArea);
		content->setObjectName("OptionsContent");
		const bool bIsSceneSelection = operationId == LauncherScenesOperationId();
		scrollArea->setAlignment(bIsSceneSelection ? Qt::AlignTop : (Qt::AlignHCenter | Qt::AlignTop));
		content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
		if (!bIsSceneSelection)
		{
			content->setMaximumWidth(LauncherUi::Page::MaxContentWidth);
		}
		QVBoxLayout* layout = new QVBoxLayout(content);
		layout->setContentsMargins(bIsSceneSelection ? LauncherUi::Page::SceneSelectionMargins : LauncherUi::Page::ContentMargins);
		layout->setSpacing(bIsSceneSelection ? 0 : LauncherUi::Page::Spacing);
		AddOptionsForOperation(*layout, operationId);
		layout->addStretch(1);
		scrollArea->setWidget(content);
		return scrollArea;
	}

	void LauncherMainWindow::SelectWorkspaceCompiler(WorkspaceCompiler compiler)
	{
		const QString value = QString::fromStdString(WorkspaceCompilerCommandLineValue(compiler));
		m_settings.SetWorkspaceCompiler(value);
		if (m_workspaceCompilerCombo == nullptr)
		{
			return;
		}

		const int index = m_workspaceCompilerCombo->findData(value);
		if (index >= 0 && index != m_workspaceCompilerCombo->currentIndex())
		{
			const QSignalBlocker blocker(m_workspaceCompilerCombo);
			m_workspaceCompilerCombo->setCurrentIndex(index);
		}
	}
}
