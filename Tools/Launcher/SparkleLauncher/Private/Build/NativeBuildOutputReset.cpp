#include "Build/NativeBuildOutputReset.h"

#include "ToolOutputPaths.h"

#include <string_view>
#include <system_error>

namespace SparkleLauncher
{
	static bool ValidateBuildDirectoryResetScope(const std::filesystem::path& workspaceRoot, const std::filesystem::path& buildDirectory, std::string& errorMessage)
	{
		if (workspaceRoot.empty() || buildDirectory.empty())
		{
			errorMessage = "Cannot reset native build outputs without workspace and build directory paths.";
			return false;
		}

		std::error_code errorCode;
		const std::filesystem::path normalizedWorkspaceRoot = std::filesystem::weakly_canonical(workspaceRoot, errorCode);
		if (errorCode)
		{
			errorMessage = "Failed to resolve the workspace root before resetting native build outputs: " + errorCode.message();
			return false;
		}

		errorCode.clear();
		const std::filesystem::path normalizedBuildDirectory = std::filesystem::weakly_canonical(buildDirectory, errorCode);
		if (errorCode)
		{
			errorMessage = "Failed to resolve the build directory before resetting native build outputs: " + errorCode.message();
			return false;
		}

		errorCode.clear();
		const std::filesystem::path repositoryRelativeToBuildDirectory = std::filesystem::relative(normalizedWorkspaceRoot, normalizedBuildDirectory, errorCode);
		const bool bContainsRepositoryEmpty = !errorCode && !repositoryRelativeToBuildDirectory.empty();
		const bool bContainsRepository = bContainsRepositoryEmpty && (*repositoryRelativeToBuildDirectory.begin() != "..");
		const bool bUnsafeBuildDirectoryRootpath = normalizedBuildDirectory.empty() || normalizedBuildDirectory == normalizedBuildDirectory.root_path();
		const bool bUnsafeBuildDirectory = bUnsafeBuildDirectoryRootpath || (bContainsRepository);

		if (bUnsafeBuildDirectory)
		{
			errorMessage = "Refusing to reset native build outputs because the configured build directory contains the workspace: " + normalizedBuildDirectory.string();
			return false;
		}
		return true;
	}

	static bool RemoveGeneratedPath(const std::filesystem::path& path, std::string& errorMessage)
	{
		std::error_code errorCode;
		if (!std::filesystem::exists(path, errorCode))
		{
			if (!errorCode)
			{
				return true;
			}

			errorMessage = "Failed to inspect incompatible build output: " + path.string() + ": " + errorCode.message();
			return false;
		}

		std::filesystem::remove_all(path, errorCode);
		if (!errorCode)
		{
			return true;
		}

		errorMessage = "Failed to remove incompatible build output: " + path.string() + ": " + errorCode.message();
		return false;
	}

	template <typename Action>
	static bool CleanGeneratedDirectory(const std::filesystem::path& directory, std::string_view scope, Action&& action, std::string& errorMessage)
	{
		std::error_code error;
		if (!std::filesystem::is_directory(directory, error))
		{
			if (!error)
			{
				return true;
			}
			errorMessage = "Failed to inspect " + std::string(scope) + ": " + directory.string() + ": " + error.message();
			return false;
		}

		std::filesystem::directory_iterator entry(directory, error);
		const std::filesystem::directory_iterator end;
		while (!error && entry != end)
		{
			const std::filesystem::path path = entry->path();
			entry.increment(error);
			if (!action(path))
			{
				return false;
			}
		}
		if (error)
		{
			errorMessage = "Failed to enumerate " + std::string(scope) + ": " + directory.string() + ": " + error.message();
			return false;
		}
		return true;
	}

	static bool RemoveDependencyBuildState(const std::filesystem::path& dependencyRoot, std::string& errorMessage)
	{
		return CleanGeneratedDirectory(
		    dependencyRoot,
		    "source dependency cache",
		    [&](const std::filesystem::path& path)
		    {
			    const std::string name = path.filename().string();
			    const bool bGenerated = name.ends_with("-build") || name.ends_with("-subbuild");
			    return !bGenerated || RemoveGeneratedPath(path, errorMessage);
		    },
		    errorMessage);
	}

	static bool RemoveGeneratedBuildTree(const std::filesystem::path& buildDirectory, const std::filesystem::path& dependencyCacheRoot, std::string& errorMessage)
	{
		return CleanGeneratedDirectory(
		    buildDirectory,
		    "generated build tree",
		    [&](const std::filesystem::path& path)
		    {
			    if (path.lexically_normal() == dependencyCacheRoot.lexically_normal())
			    {
				    return RemoveDependencyBuildState(path, errorMessage);
			    }
			    return RemoveGeneratedPath(path, errorMessage);
		    },
		    errorMessage);
	}

	static bool RemoveProjectBuildOutputs(const SparkleTools::ToolOutputPaths& outputs, std::string& errorMessage)
	{
		return CleanGeneratedDirectory(
		    outputs.ProjectArtifactRoot,
		    "project artifacts",
		    [&](const std::filesystem::path& projectDirectory)
		    {
			    std::error_code error;
			    const bool bDirectory = std::filesystem::is_directory(projectDirectory, error);
			    if (error)
			    {
				    errorMessage = "Failed to enumerate project artifacts: " + outputs.ProjectArtifactRoot.string() + ": " + error.message();
				    return false;
			    }
			    if (!bDirectory)
			    {
				    return RemoveGeneratedPath(projectDirectory, errorMessage);
			    }

			    const std::filesystem::path cookedDirectory = outputs.CookedProjectDirectory(projectDirectory.filename().string()).lexically_normal();

			    return CleanGeneratedDirectory(
			        projectDirectory,
			        "project artifacts",
			        [&](const std::filesystem::path& path) { return path.lexically_normal() == cookedDirectory || RemoveGeneratedPath(path, errorMessage); },
			        errorMessage);
		    },
		    errorMessage);
	}

	static bool RemoveCompiledDevelopmentArtifacts(const SparkleTools::ToolOutputPaths& outputs, std::string& errorMessage)
	{
		const bool bProductOutputsRemoved = CleanGeneratedDirectory(
		    outputs.ProductRoot,
		    "development artifacts",
		    [&](const std::filesystem::path& path)
		    {
			    return path.lexically_normal() == outputs.ProjectArtifactRoot.lexically_normal() || RemoveGeneratedPath(path, errorMessage);
		    },
		    errorMessage);

		return bProductOutputsRemoved && RemoveProjectBuildOutputs(outputs, errorMessage);
	}

	bool RequiresNativeBuildOutputReset(BuildFilesFreshnessState state)
	{
		return state == BuildFilesFreshnessState::GeneratorMismatch || state == BuildFilesFreshnessState::FreshnessStampMismatch;
	}

	bool ResetNativeBuildOutputs(const std::filesystem::path& workspaceRoot, const std::filesystem::path& buildDirectory, std::string& errorMessage)
	{
		errorMessage.clear();
		const SparkleTools::ToolOutputPaths outputs = SparkleTools::ResolveToolOutputPaths(workspaceRoot);
		if (!ValidateBuildDirectoryResetScope(workspaceRoot, buildDirectory, errorMessage))
		{
			return false;
		}
		if (!RemoveGeneratedBuildTree(buildDirectory, outputs.DependencyCacheRoot, errorMessage))
		{
			return false;
		}

		if (!RemoveCompiledDevelopmentArtifacts(outputs, errorMessage))
		{
			return false;
		}
		return RemoveGeneratedPath(outputs.SymbolsRoot, errorMessage);
	}
}
