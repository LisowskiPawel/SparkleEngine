#include "LevelRunOperationProcessRequests.h"

#include "LauncherStatePaths.h"
#include "GpuCrashLaunch.h"
#include "Core/Public/Strings/StringUtils.h"

#include <utility>

namespace SparkleLauncher
{
	std::vector<LevelRunOperationProcessStep> BuildLevelRunProcessStepsForPlan(const LevelRunOperationPlan& plan)
	{
		std::vector<LevelRunOperationProcessStep> steps;
		if (!plan.CanRun)
		{
			return steps;
		}

		Process::ChildProcessRequest request;
		request.ExecutablePath = plan.ExecutablePath;
		request.WorkingDirectory = plan.WorkingDirectory;
		request.Environment = plan.Environment;
		request.LogPath = ResolveLauncherOperationLogPath(plan.RepositoryRoot, plan.Operation.Id, "RunLevel.txt");
		request.Arguments = {"--graphics-api", plan.Request.GraphicsApi};
		if (plan.Request.ProductProfile == "DebugEditor" || plan.Request.ProductProfile == "DevelopmentEditor")
		{
			request.Arguments.emplace_back("--capture-provider");
			request.Arguments.emplace_back(ExternalCaptureProviderToString(plan.Request.CaptureProvider));
		}
		if (plan.Request.GpuCrashMode == RhiGpuCrashMode::Aftermath)
		{
			request.Arguments.insert(
			    request.Arguments.end(),
			    {"--gpu-crash-debugging", "aftermath", "--shader-export", Strings::ToNarrow(plan.Request.ShaderExportPath.wstring()), "--gpu-crash-session", plan.SessionId});

			const GpuCrashReadinessExpectation expected{plan.ExecutablePath, plan.Request.ShaderExportPath, plan.SessionId, plan.Request.ProductProfile, plan.Request.GraphicsApi};

			request.ReadinessValidator = [expected](std::uint32_t pid, std::string_view output, std::string& error)
			{
				return VerifyGpuCrashReadiness(expected, pid, output, error);
			};
		}
		request.ReadinessValue = plan.Request.LevelId;

		LevelRunOperationProcessStep step;
		step.Id = "run-level";
		step.DisplayName = (plan.Request.RunMode == LevelRunMode::Editor ? "Open " : "Run ") + plan.Request.LevelId;
		step.Request = std::move(request);
		steps.push_back(std::move(step));
		return steps;
	}
}
