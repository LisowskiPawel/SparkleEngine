#include "Gui/Models/LauncherUiModel.h"

namespace SparkleLauncher
{

	struct OperationUiDefinition final
	{
		const char* Id;
		const char* DisplayName;
		const char* PrimaryVerb;
		LauncherWorkflowPageKind PageKind;
		LauncherActionImpactKind ImpactKind;
		const char* ImpactText;
		const char* VisualTitle;
		const char* VisualText;
		const char* VisualAssetName;
	};

	static constexpr OperationUiDefinition ExactOperations[] = {
	    {"levels.browse",
	        "Scenes",
	        "Scenes",
	        LauncherWorkflowPageKind::Scenes,
	        LauncherActionImpactKind::None,
	        {},
	        "Explore Sparkle",
	        "Choose a scene and open Editor or Game using its existing binary and cooked files.",
	        "workflow-scenes.png"},
	    {"levels.run",
	        "Run Level",
	        "Run",
	        LauncherWorkflowPageKind::Scenes,
	        LauncherActionImpactKind::LevelAssets,
	        "Open the selected level using its existing binary and cooked files. Build and Cook remain explicit developer actions.",
	        "Open level",
	        "Uses the level selected from its scene card.",
	        "workflow-scenes.png"},
	    {"workspace.sync-code",
	        "Sync Code",
	        "Sync All",
	        LauncherWorkflowPageKind::Sync,
	        LauncherActionImpactKind::SourceDependencies,
	        "Downloads and repairs enabled repository dependencies, refreshes workspace configure state, and exposes registered "
	        "host-tool "
	        "setup actions.",
	        "Workspace setup",
	        "Fetch the repository packages needed for local builds, content tools, and optional renderer integrations.",
	        "workflow-source-sync.png"},
	    {"workspace.generate-build-files",
	        "Generate Build Files",
	        "Generate",
	        LauncherWorkflowPageKind::Build,
	        LauncherActionImpactKind::WorkspaceFiles,
	        "Build files: refreshes generated CMake and IDE build-system state without building products.",
	        "Workspace files",
	        "Refreshes workspace files for the selected toolchain and opens the IDE when the workspace is current.",
	        "workflow-generate-build-files.png"},
	    {"workspace.build",
	        "Build Workspace",
	        "Build",
	        LauncherWorkflowPageKind::Build,
	        LauncherActionImpactKind::BuildOutputs,
	        "Build outputs: incrementally builds the selected products and refreshes generated workspace files when required.",
	        "Selected products",
	        "Choose the editor, game, cooking tools, or launcher, then build them as one dependency-aware request.",
	        "workflow-build-all.png"},
	    {"levels.sync",
	        "Sync Levels",
	        "Sync",
	        LauncherWorkflowPageKind::Scenes,
	        LauncherActionImpactKind::LevelAssets,
	        "Levels: syncs or cleans maps and their asset packs without changing code or SDK dependencies.",
	        "Levels and assets",
	        "Sync individual levels or the full catalog from declared publisher sources.",
	        "workflow-cook-assets.png"},
	    {"launcher.build.self",
	        "Build Launcher",
	        "Build",
	        LauncherWorkflowPageKind::Build,
	        LauncherActionImpactKind::BuildOutputs,
	        "Build outputs: optional local rebuild that refreshes the launcher artifact for development work.",
	        "Build outputs",
	        "Creates the local launcher artifact used for development.",
	        "workflow-launcher-build.png"},
	    {"workspace.build.editor",
	        "Build Editor",
	        "Build",
	        LauncherWorkflowPageKind::Build,
	        LauncherActionImpactKind::BuildOutputs,
	        "Build outputs: optional local rebuild of the editor for development work.",
	        "Build outputs",
	        "Creates the local editor artifact used for development work.",
	        "workflow-editor-build.png"},
	    {"workspace.build.game",
	        "Build Game",
	        "Build",
	        LauncherWorkflowPageKind::Build,
	        LauncherActionImpactKind::BuildOutputs,
	        "Build outputs: optional local rebuild of the game for development work.",
	        "Build outputs",
	        "Creates the local game artifact used for development work.",
	        "workflow-game-build.png"},
	    {"cook.tools.prepare",
	        "Build Cook Tools",
	        "Build",
	        LauncherWorkflowPageKind::Build,
	        LauncherActionImpactKind::BuildOutputs,
	        "Build outputs: prepares local cook tools for asset refresh workflows.",
	        "Build outputs",
	        "Builds the tools used by explicit cook actions.",
	        "workflow-cook-tools.png"},
	    {"cook.all",
	        "Cook All",
	        "Cook",
	        LauncherWorkflowPageKind::Cook,
	        LauncherActionImpactKind::CookedOutputs,
	        "Cooked outputs: optional local recook that refreshes all generated content.",
	        "Content outputs",
	        "Refreshes cooked content for selected catalog levels.",
	        "workflow-cook-all.png"},
	    {"cook.workspace",
	        "Cook Workspace",
	        "Cook",
	        LauncherWorkflowPageKind::Cook,
	        LauncherActionImpactKind::CookedOutputs,
	        "Cooked outputs: incrementally refreshes the selected shader, texture, and scene content.",
	        "Selected content outputs",
	        "Choose shaders, textures, or scene assets, then cook them as one ordered request.",
	        "workflow-cook-all.png"},
	    {"cook.shaders",
	        "Cook Shaders",
	        "Cook",
	        LauncherWorkflowPageKind::Cook,
	        LauncherActionImpactKind::CookedOutputs,
	        "Cooked outputs: optional local recook that refreshes generated shader content.",
	        "Content outputs",
	        "Refreshes cooked shader outputs for selected catalog levels.",
	        "workflow-cook-shaders.png"},
	    {"cook.textures",
	        "Cook Textures",
	        "Cook",
	        LauncherWorkflowPageKind::Cook,
	        LauncherActionImpactKind::CookedOutputs,
	        "Cooked outputs: optional local recook that refreshes generated texture content.",
	        "Content outputs",
	        "Refreshes cooked texture outputs for selected catalog levels.",
	        "workflow-cook-textures.png"},
	    {"cook.assets",
	        "Cook Scene Assets",
	        "Cook",
	        LauncherWorkflowPageKind::Cook,
	        LauncherActionImpactKind::CookedOutputs,
	        "Cooked outputs: optional local recook that refreshes generated scene content.",
	        "Content outputs",
	        "Refreshes cooked scene outputs for selected catalog levels.",
	        "workflow-cook-assets.png"},
	    {"workspace.clean",
	        "Clean Workspace",
	        "Clean",
	        LauncherWorkflowPageKind::Clean,
	        LauncherActionImpactKind::GeneratedState,
	        "Clean: removes selected generated outputs, caches, logs, or local workspace state after confirmation.",
	        "Generated files",
	        "Shows what generated state will be removed and what will stay before destructive cleanup.",
	        "workflow-clean-workspace.png"},
	};

	static constexpr OperationUiDefinition OperationFamilies[] = {
	    {"workspace.build.",
	        {},
	        "Build",
	        LauncherWorkflowPageKind::Build,
	        LauncherActionImpactKind::BuildOutputs,
	        "Build outputs: optional local rebuild of the selected development target.",
	        "Build outputs",
	        "Creates the selected local development artifacts.",
	        "workflow-build-generic.png"},
	    {"cook.",
	        {},
	        "Cook",
	        LauncherWorkflowPageKind::Cook,
	        LauncherActionImpactKind::CookedOutputs,
	        "Cooked outputs: optional local recook that refreshes generated content.",
	        "Content outputs",
	        "Refreshes cooked content for selected catalog levels.",
	        "workflow-cook-generic.png"},
	};

	static LauncherOperationUiModel MakeOperationUiModel(const QString& operationId, const OperationUiDefinition& definition)
	{
		LauncherOperationUiModel model;
		model.OperationId = operationId;
		model.DisplayName = definition.DisplayName ? QString::fromUtf8(definition.DisplayName) : QString{};
		model.PrimaryVerb = QString::fromUtf8(definition.PrimaryVerb);
		model.PageKind = definition.PageKind;
		model.ImpactKind = definition.ImpactKind;
		model.ImpactText = definition.ImpactText ? QString::fromUtf8(definition.ImpactText) : QString{};
		model.VisualTitle = QString::fromUtf8(definition.VisualTitle);
		model.VisualText = QString::fromUtf8(definition.VisualText);
		model.VisualAssetName = QString::fromUtf8(definition.VisualAssetName);
		return model;
	}

	LauncherOperationUiModel UnknownOperationUiModel(const QString& operationId)
	{
		LauncherOperationUiModel model;
		model.OperationId = operationId;
		model.PrimaryVerb = "Run";
		model.VisualTitle = "Workflow";
		model.VisualText = "Context artwork is informational; the primary action remains in the workflow controls below.";
		model.VisualAssetName = "workflow-fallback-tools.png";
		return model;
	}

	LauncherOperationUiModel LauncherUiModelForOperation(const QString& operationId)
	{
		for (const OperationUiDefinition& definition : ExactOperations)
		{
			if (operationId == definition.Id)
			{
				return MakeOperationUiModel(operationId, definition);
			}
		}
		for (const OperationUiDefinition& definition : OperationFamilies)
		{
			if (operationId.startsWith(definition.Id))
			{
				return MakeOperationUiModel(operationId, definition);
			}
		}
		return UnknownOperationUiModel(operationId);
	}

	QString LauncherWorkflowPageKindName(LauncherWorkflowPageKind pageKind)
	{
		switch (pageKind)
		{
			case LauncherWorkflowPageKind::Scenes:
				return "Scenes";
			case LauncherWorkflowPageKind::Sync:
				return "Sync";
			case LauncherWorkflowPageKind::Build:
				return "Build";
			case LauncherWorkflowPageKind::Cook:
				return "Cook";
			case LauncherWorkflowPageKind::Clean:
				return "Clean";
			case LauncherWorkflowPageKind::Unknown:
				return "Unknown";
		}
		return "Unknown";
	}

	QString LauncherActionImpactKindName(LauncherActionImpactKind impactKind)
	{
		switch (impactKind)
		{
			case LauncherActionImpactKind::None:
				return "None";
			case LauncherActionImpactKind::SourceDependencies:
				return "Source dependencies";
			case LauncherActionImpactKind::LevelAssets:
				return "Level assets";
			case LauncherActionImpactKind::WorkspaceFiles:
				return "Workspace files";
			case LauncherActionImpactKind::BuildOutputs:
				return "Build outputs";
			case LauncherActionImpactKind::CookedOutputs:
				return "Cooked outputs";
			case LauncherActionImpactKind::GeneratedState:
				return "Generated state";
		}
		return "Unknown";
	}
}
