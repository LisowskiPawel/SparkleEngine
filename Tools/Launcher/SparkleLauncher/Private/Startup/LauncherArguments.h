#pragma once

#include "SparkleLauncher/BuildWorkspaceOperations.h"
#include "SparkleLauncher/CookOperations.h"
#include "SparkleLauncher/MaintenanceOperations.h"
#include "SparkleLauncher/LevelRunOperations.h"

#include <filesystem>
#include <iosfwd>
#include <string>

namespace SparkleLauncher
{
	struct LauncherShellArguments final
	{
		std::filesystem::path StartPath;
		std::string EditorProfile = "DevelopmentEditor";
		std::string GameProfile = "DevelopmentGame";
		WorkspaceIde WorkspaceIdePreference = WorkspaceIde::VisualStudio;
		WorkspaceCompiler WorkspaceCompilerPreference = WorkspaceCompiler::Msvc;
		std::string RunOperationId;
		std::string LevelId;
		LevelRunMode Product = LevelRunMode::Editor;
		std::string GraphicsApi = "d3d12";
		RhiGpuCrashMode GpuCrashMode = RhiGpuCrashMode::Off;
		std::filesystem::path ShaderExportPath;
		CookMode RequestedCookMode = CookMode::Incremental;
		CleanScope RequestedCleanScope = CleanScope::CookedOutputs;
		bool bForceRecookConfirmed = false;
		bool bCleanConfirmed = false;
		bool bShowHelp = false;
		bool bShaderSymbols = false;
	};

	bool ShouldRunLauncherShell(int argc, char** argv);
	std::filesystem::path ParseLauncherGuiRoot(int argc, char** argv);

	bool ParseLauncherShellArguments(int argc, char** argv, LauncherShellArguments& outArguments, std::ostream& error);
	void PrintLauncherShellUsage(std::ostream& output);
}
