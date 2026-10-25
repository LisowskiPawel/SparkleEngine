#include "LauncherShell.h"

#include "LauncherShellArguments.h"
#include "LauncherShellModel.h"
#include "LauncherShellOperations.h"
#include "LauncherShellPresentation.h"
#include "LauncherShadowExecution.h"
#include "LauncherRootResolver.h"
#include "Core/Public/FileSystemUtils.h"

#include "SparkleLauncher/ProjectResolver.h"
#include "SparkleLauncher/RepositoryLocator.h"

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

		if (arguments.ShowHelp)
		{
			PrintLauncherShellUsage(output);
			return 0;
		}

		std::string errorMessage;
		const std::optional<std::filesystem::path> repository = TryResolveLauncherRoot(arguments.StartPath, Filesystem::GetExecutableDirectory(), errorMessage);
		if (!repository.has_value())
		{
			error << errorMessage << '\n';
			return 1;
		}

		if (!arguments.RunOperationId.empty())
		{
			LauncherShadowCompletionPolicy completionPolicy = LauncherShadowCompletionPolicy::WaitForCompletion;
			const std::optional<BuildWorkspaceOperationDefinition> buildOperation = FindBuildWorkspaceOperationDefinition(arguments.RunOperationId);
			if (buildOperation.has_value() && buildOperation->ReplacesLauncherArtifact)
			{
				completionPolicy = LauncherShadowCompletionPolicy::ReleaseCallingArtifact;
			}

			std::vector<std::string> shadowArguments;
			for (int index = 1; index < argc; ++index)
			{
				shadowArguments.emplace_back(argv[index]);
			}
			const LauncherShadowStartResult shadow = StartLauncherShadow(*repository, shadowArguments, completionPolicy);
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

		std::optional<LauncherProject> content = ResolveLauncherProject(*repository, errorMessage);
		if (!content.has_value())
		{
			error << errorMessage << '\n';
			return 1;
		}

		LauncherShellModel model = BuildLauncherShellModel(*repository, std::move(*content), arguments);
		if (!arguments.RunOperationId.empty())
		{
			return RunLauncherShellOperation(model, arguments, output, error);
		}
		const bool previewSucceeded = arguments.DryRunOperationId.empty() || ApplyLauncherShellDryRun(model, arguments);
		RenderLauncherShell(model, output);
		return previewSucceeded ? 0 : 1;
	}
}
