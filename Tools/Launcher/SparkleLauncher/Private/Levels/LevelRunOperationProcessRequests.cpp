#include "Levels/LevelRunOperationProcessRequests.h"

#include "Workspace/LauncherStatePaths.h"
#include "GpuCrash/GpuCrashLaunch.h"
#include "Core/Public/Strings/StringUtils.h"

namespace SparkleLauncher
{
	void ConfigureLevelRunStep(LevelRunOperationPlan& plan)
	{
		Process::ChildProcessRequest& request = plan.Step.Request;
		request.LogPath = ResolveLauncherOperationLogPath(plan.Request.WorkspaceRoot, plan.Operation.Id, "RunLevel.txt");
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

			const GpuCrashReadinessExpectation expected{request.ExecutablePath, plan.Request.ShaderExportPath, plan.SessionId, plan.Request.ProductProfile, plan.Request.GraphicsApi};

			request.ReadinessValidator = [expected](std::uint32_t pid, std::string_view output, std::string& error)
			{
				return VerifyGpuCrashReadiness(expected, pid, output, error);
			};
		}
		request.ReadinessValue = plan.Request.LevelId;

		plan.Step.DisplayName = (plan.Request.RunMode == LevelRunMode::Editor ? "Open " : "Run ") + plan.Request.LevelId;
	}
}
