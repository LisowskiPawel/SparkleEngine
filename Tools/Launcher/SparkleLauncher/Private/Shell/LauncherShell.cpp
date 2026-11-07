#include "Shell/LauncherShell.h"

#include "Shell/LauncherShellArguments.h"
#include "Shell/LauncherShellModel.h"
#include "Shell/LauncherShellOperations.h"
#include "Shell/LauncherShellPresentation.h"
#include "Startup/LauncherShadowExecution.h"
#include "Workspace/LauncherRootResolver.h"
#include "Core/Public/Paths/FileSystemPaths.h"

#include "SparkleLauncher/ProjectResolver.h"
#include "SparkleLauncher/WorkspaceLocator.h"

#include <filesystem>
#include <optional>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace SparkleLauncher
{
	int LauncherShell::Run(int argc, char** argv, std::ostream& output, std::ostream& error) const
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

		std::string errorMessage;
		const std::optional<std::filesystem::path> workspace = TryResolveLauncherRoot(arguments.StartPath, Filesystem::GetExecutableDirectory(), errorMessage);
		if (!workspace.has_value())
		{
			error << errorMessage << '\n';
			return 1;
		}

		if (!arguments.RunOperationId.empty())
		{
			LauncherShadowCompletionPolicy completionPolicy = LauncherShadowCompletionPolicy::WaitForCompletion;
			const std::optional<BuildWorkspaceOperationDefinition> buildOperation = FindBuildWorkspaceOperationDefinition(arguments.RunOperationId);
			if (buildOperation.has_value() && buildOperation->bReplacesLauncherArtifact)
			{
				completionPolicy = LauncherShadowCompletionPolicy::ReleaseCallingArtifact;
			}

			std::vector<std::string> shadowArguments;
			for (int index = 1; index < argc; ++index)
			{
				shadowArguments.emplace_back(argv[index]);
			}
			const LauncherShadowStartResult shadow = StartLauncherShadow(*workspace, shadowArguments, completionPolicy);
			if (shadow.State == LauncherShadowStartState::Started)
			{
				if (completionPolicy == LauncherShadowCompletionPolicy::ReleaseCallingArtifact)
				{
					output << "Operation handed off to the per-user Launcher shadow; progress and result are recorded in Launcher logs.\n";
				}
				return shadow.ExitCode;
			}
			if (shadow.State == LauncherShadowStartState::Failed)
			{
				error << shadow.ErrorMessage << '\n';
				return 1;
			}
		}

		std::optional<LauncherProject> content = ResolveLauncherProject(*workspace, errorMessage);
		if (!content.has_value())
		{
			error << errorMessage << '\n';
			return 1;
		}

		LauncherShellModel model = BuildLauncherShellModel(*workspace, std::move(*content), arguments);
		if (!arguments.RunOperationId.empty())
		{
			return RunLauncherShellOperation(model, arguments, output, error);
		}
		const bool bPreviewSucceeded = arguments.DryRunOperationId.empty() || ApplyLauncherShellDryRun(model, arguments);
		RenderLauncherShell(model, output);
		return bPreviewSucceeded ? 0 : 1;
	}
}
