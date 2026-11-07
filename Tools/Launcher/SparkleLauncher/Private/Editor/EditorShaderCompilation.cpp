#include "SparkleLauncher/EditorShaderCompilation.h"

#include "SparkleLauncher/BuildProfileCatalog.h"
#include "ToolOutputPaths.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Paths/ProductUserStatePaths.h"
#include "Core/Public/Process/ChildProcess.h"
#include "Core/Public/Strings/StringUtils.h"

#include <algorithm>
#include <string>

namespace SparkleLauncher
{
	ConsoleCommandResult RecompileEditorShaders(bool bGenerateSymbols, std::stop_token cancellation)
	{
		const auto editorProfile = FindBuildProfile(Filesystem::GetExecutableDirectory().filename().string());
		if (!editorProfile || editorProfile->Target != BuildProfileTarget::Editor)
		{
			return ConsoleCommandResult::Error("Editor shader compilation requires the normal editor binary directory.");
		}
		const auto& profiles = GetBuildProfileCatalog();
		const auto gameProfile = std::ranges::find_if(profiles, [&](const BuildProfile& profile) { return profile.State == editorProfile->State && profile.Target == BuildProfileTarget::Game; });
		if (gameProfile == profiles.end())
		{
			return ConsoleCommandResult::Error("No matching shader cook profile.");
		}

		const auto& root = Filesystem::GetWorkspaceRootPath();
		Process::ChildProcessRequest request;
		request.ExecutablePath = SparkleTools::ResolveToolOutputPaths(root).LauncherTargetOutputs(editorProfile->Name).BinaryDirectory / "SparkleLauncher.exe";
		request.WorkingDirectory = root;
		request.LogPath = Filesystem::GetProductUserStatePaths().LogsRoot / "ShaderCompile.txt";
		request.Cancellation = cancellation;
		request.Arguments = {"--root", Strings::ToNarrow(root.wstring()), "--editor-profile", editorProfile->Name, "--game-profile", gameProfile->Name, "--run", "cook.shaders"};
		if (bGenerateSymbols)
		{
			request.Arguments.push_back("--shader-symbols");
		}
		const auto result = Process::RunChildProcess(request);
		if (!result.Succeeded())
		{
			if (result.StartFailure == Process::ChildProcessStartFailure::ExecutableNotFound)
			{
				return ConsoleCommandResult::Error("Build Launcher for " + editorProfile->Name + " before recompiling from Editor. " + result.FailureReason);
			}

			return ConsoleCommandResult::Error(
			    "Shader compilation failed; see " + Strings::ToNarrow(request.LogPath.wstring()) + ".\n" + result.FailureReason + "\n"
			    + result.CapturedOutput.substr(result.CapturedOutput.size() > 2048 ? result.CapturedOutput.size() - 2048 : 0));
		}
		return ConsoleCommandResult::Success(bGenerateSymbols ? "Shaders recompiled with private symbols." : "Shaders recompiled.");
	}
}
