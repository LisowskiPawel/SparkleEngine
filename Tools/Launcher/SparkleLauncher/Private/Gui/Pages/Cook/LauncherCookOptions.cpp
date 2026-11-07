#include "Gui/Pages/Cook/LauncherCookOptions.h"
#include "Gui/Widgets/LauncherOptionWidgets.h"
#include "Gui/Widgets/LauncherSelectionWidgets.h"
#include "Gui/App/LauncherSettings.h"
#include "Gui/Style/LauncherUiDesign.h"
#include "SparkleLauncher/BuildWorkspaceOperations.h"

#include <QtCore/QRegularExpression>
#include <QtCore/QStringList>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace SparkleLauncher
{
	void BuildLauncherCookOptions(QVBoxLayout& layout, LauncherSettings& settings, const std::function<void(QWidget*)>& registerFocusable, const std::function<void()>& selectionChanged)
	{
		const WorkspaceFeatureSettings features = GetLauncherWorkspaceFeatureSettings();
		const QStringList selectedScopes = settings.CookScopes().split(QRegularExpression("[,;\\n]"), Qt::SkipEmptyParts);
		QVector<QCheckBox*> scopeBoxes;

		QVBoxLayout* cookLayout = AddOptionGroup(layout, "Choose outputs", "Select the cooked content to prepare.");

		QFrame* selectionPanel = new QFrame(cookLayout->parentWidget());
		selectionPanel->setObjectName("WorkflowSelectionPanel");
		selectionPanel->setMinimumWidth(LauncherUi::ScopeSelection::ContentMinWidth);
		selectionPanel->setMaximumWidth(LauncherUi::ScopeSelection::ContentMaxWidth);
		QVBoxLayout* selectionLayout = new QVBoxLayout(selectionPanel);
		selectionLayout->setContentsMargins(0, 0, 0, 0);
		selectionLayout->setSpacing(0);
		QVector<QCheckBox*> selectAllBoxes;
		AddWorkflowScopeRow(*selectionLayout, "All", QString(), "Select every available output.", QString(), true, QStringList(), selectAllBoxes, registerFocusable);
		QCheckBox* selectAllBox = selectAllBoxes.front();

		AddWorkflowScopeRow(
		    *selectionLayout,
		    "Shaders",
		    "shaders",
		    features.bShaderCompilerEnabled ? "Compile every registered shader into the cooked map and code library." : "Shader cooking is not enabled by this workspace configuration.",
		    features.bShaderCompilerEnabled ? QStringLiteral("DXIL + SPIR-V") : QStringLiteral("Unavailable"),
		    features.bShaderCompilerEnabled,
		    selectedScopes,
		    scopeBoxes,
		    registerFocusable);

		AddWorkflowScopeRow(
		    *selectionLayout,
		    "Textures",
		    "textures",
		    features.bContentPipelineEnabled ? "Create cooked texture products." : "Texture cooking is not enabled by this workspace configuration.",
		    features.bContentPipelineEnabled ? QStringLiteral("Asset + Texture cooker") : QStringLiteral("Unavailable"),
		    features.bContentPipelineEnabled,
		    selectedScopes,
		    scopeBoxes,
		    registerFocusable);

		AddWorkflowScopeRow(
		    *selectionLayout,
		    "Scene assets",
		    "assets",
		    features.bContentPipelineEnabled ? "Cook scenes, meshes, and materials." : "Scene-asset cooking is not enabled by this workspace configuration.",
		    features.bContentPipelineEnabled ? QStringLiteral("Asset cooker") : QStringLiteral("Unavailable"),
		    features.bContentPipelineEnabled,
		    selectedScopes,
		    scopeBoxes,
		    registerFocusable);

		cookLayout->addWidget(selectionPanel);
		QCheckBox* shaderSymbols = new QCheckBox("Generate shader symbols", cookLayout->parentWidget());
		shaderSymbols->setChecked(settings.ShaderSymbols());
		shaderSymbols->setEnabled(features.bShaderCompilerEnabled);
		shaderSymbols->setToolTip("Export private shader debug data. Runtime optimization is unchanged.");
		registerFocusable(shaderSymbols);
		cookLayout->addWidget(shaderSymbols);
		QObject::connect(shaderSymbols, &QCheckBox::toggled, &settings, &LauncherSettings::SetShaderSymbols);

		AddWorkflowAutomationNote(
		    *cookLayout,
		    selectedScopes.contains("shaders") ? QStringLiteral("ShaderCompiler discovers typed shaders and canonical targets. Incremental cooking preserves unaffected map entries.")
		                                       : QStringLiteral("Incremental cooking reuses current outputs."));

		const auto commitSelection = [&settings, scopeBoxes, selectionChanged](bool)
		{
			const QStringList selectedValues = CollectSelectedScopeValues(scopeBoxes);
			settings.SetCookScopes(selectedValues.join(';'));
			selectionChanged();
		};

		ConnectSelectAllScopeBox(selectAllBox, scopeBoxes, layout.parentWidget(), commitSelection);
		commitSelection(false);
	}
}
