#include "Shell/LauncherShell.h"

#include "Startup/LauncherArguments.h"
#include "Operations/LauncherOperationExecution.h"
#include "Workspace/LauncherRootResolver.h"
#include "Workspace/LauncherProductPaths.h"
#include "SparkleLauncher/ProjectResolver.h"
#include "Core/Public/Paths/FileSystemPaths.h"

#include <optional>
#include <ostream>

namespace SparkleLauncher
{
	static std::optional<LauncherOperationInputs> BuildLauncherShellInputs(const std::filesystem::path& workspaceRoot, const std::string& contentId, const LauncherShellArguments& arguments)
	{
		const auto& operationId = arguments.RunOperationId;
		const bool bSourceWorkspace = HasBuildSources(workspaceRoot);
		const std::string editorProfile = bSourceWorkspace ? arguments.EditorProfile : "ShippingEditor";
		const std::string gameProfile = bSourceWorkspace ? arguments.GameProfile : "ShippingGame";
		if (FindBuildWorkspaceOperationDefinition(operationId) != nullptr)
		{
			return BuildWorkspaceOperationRequest{
			    .WorkspaceRoot = workspaceRoot,
			    .ContentId = contentId,
			    .EditorProfile = editorProfile,
			    .GameProfile = gameProfile,
			    .PreferredIde = arguments.WorkspaceIdePreference,
			    .Compiler = arguments.WorkspaceCompilerPreference};
		}
		if (FindCookOperationDefinition(operationId) != nullptr)
		{
			CookOperationRequest request;
			request.WorkspaceRoot = workspaceRoot;
			request.ContentId = contentId;
			request.GameProfile = gameProfile;
			request.Mode = arguments.RequestedCookMode;
			request.bShaderEnableDebugInfo = arguments.bShaderSymbols;
			request.bForceRecookConfirmed = arguments.bForceRecookConfirmed;
			request.SelectedScopes = {CookWorkspaceScope::Shaders, CookWorkspaceScope::Textures, CookWorkspaceScope::SceneAssets};
			return request;
		}
		if (FindLevelOperationDefinition(operationId) != nullptr)
		{
			LevelOperationRequest request{.WorkspaceRoot = workspaceRoot, .ContentId = contentId};
			if (!arguments.LevelId.empty())
			{
				request.RequestedLevelIds = {arguments.LevelId};
			}
			return request;
		}
		if (FindLevelRunOperationDefinition(operationId) != nullptr)
		{
			return LevelRunOperationRequest{
			    .WorkspaceRoot = workspaceRoot,
			    .ContentId = contentId,
			    .RunMode = arguments.Product,
			    .ProductProfile = arguments.Product == LevelRunMode::Editor ? editorProfile : gameProfile,
			    .LevelId = arguments.LevelId,
			    .GraphicsApi = arguments.GraphicsApi,
			    .GpuCrashMode = arguments.GpuCrashMode,
			    .ShaderExportPath = arguments.ShaderExportPath};
		}
		if (FindMaintenanceOperationDefinition(operationId) != nullptr)
		{
			return MaintenanceOperationRequest{
			    .WorkspaceRoot = workspaceRoot,
			    .ContentId = contentId,
			    .EditorProfile = editorProfile,
			    .RequestedCleanScope = arguments.RequestedCleanScope,
			    .bDestructiveActionConfirmed = arguments.bCleanConfirmed};
		}
		return std::nullopt;
	}

	static int ReportLauncherOperationResult(const OperationRecord& operation, std::ostream& output, std::ostream& error)
	{
		output << "Result: " << ToString(operation.Status) << "\nOperation: " << operation.DisplayName << '\n';
		if (operation.Failure)
		{
			error << "Problem type: " << ToString(operation.Failure->Kind) << "\nWhat failed: " << operation.Failure->Summary << "\nNext action: " << operation.Failure->ExpectedAction << '\n';
		}
		std::ostream& diagnostics = operation.Failure ? error : output;
		if (operation.ExitCode)
		{
			diagnostics << "Exit code: " << *operation.ExitCode << '\n';
		}
		if (!operation.LogPath.empty())
		{
			diagnostics << "Log: " << operation.LogPath.string() << '\n';
		}
		return operation.Status == OperationStatus::Succeeded ? 0 : 1;
	}

	int RunLauncherShell(int argc, char** argv, std::ostream& output, std::ostream& error)
	{
		LauncherShellArguments arguments;
		if (!ParseLauncherShellArguments(argc, argv, arguments, error))
		{
			PrintLauncherShellUsage(error);
			return 1;
		}
		if (arguments.bShowHelp)
		{
			PrintLauncherShellUsage(output);
			return 0;
		}
		if (arguments.RunOperationId.empty())
		{
			error << "SparkleLauncher: choose an operation with --run; use --help to list operations.\n";
			return 1;
		}

		std::string failure;
		const auto executableDirectory = Filesystem::GetExecutableDirectory();
		const auto workspace = TryResolveLauncherRoot(arguments.StartPath, executableDirectory, failure);
		if (!workspace)
		{
			error << failure << '\n';
			return 1;
		}
		const auto content = ResolveLauncherProject(*workspace, failure);
		if (!content)
		{
			error << failure << '\n';
			return 1;
		}
		const auto inputs = BuildLauncherShellInputs(*workspace, content->Id, arguments);
		if (!inputs)
		{
			error << "Result: Failed\nProblem type: Planning\nWhat failed: Unknown --run operation: " << arguments.RunOperationId << "\nNext action: Use --help to choose a registered operation.\n";
			return 1;
		}

		const auto writeOutput = [&output](std::string_view text)
		{
			output << text << std::flush;
		};
		const OperationRecord result = ExecuteLauncherOperation(arguments.RunOperationId, *inputs, {}, writeOutput);
		return ReportLauncherOperationResult(result, output, error);
	}
}
