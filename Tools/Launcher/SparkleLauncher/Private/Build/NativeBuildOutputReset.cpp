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

	static bool RemoveDependencyBuildState(const std::filesystem::path& dependencyRoot, std::string& errorMessage)
	{
		std::error_code errorCode;
		if (!std::filesystem::is_directory(dependencyRoot, errorCode))
		{
			if (!errorCode)
			{
				return true;
			}

			errorMessage = "Failed to inspect source dependency cache: " + dependencyRoot.string() + ": " + errorCode.message();
			return false;
		}

		std::filesystem::directory_iterator iterator(dependencyRoot, errorCode);
		const std::filesystem::directory_iterator end;
		while (!errorCode && iterator != end)
		{
			const std::filesystem::path path = iterator->path();
			const std::string name = path.filename().string();
			iterator.increment(errorCode);
			if (name.ends_with("-build") || name.ends_with("-subbuild"))
			{
				if (!RemoveGeneratedPath(path, errorMessage))
				{
					return false;
				}
			}
		}
		if (errorCode)
		{
			errorMessage = "Failed to enumerate source dependency cache: " + dependencyRoot.string() + ": " + errorCode.message();
			return false;
		}

		return true;
	}

	static bool RemoveGeneratedBuildTree(const std::filesystem::path& buildDirectory, const std::filesystem::path& dependencyCacheRoot, std::string& errorMessage)
	{
		std::error_code errorCode;
		if (!std::filesystem::is_directory(buildDirectory, errorCode))
		{
			if (!errorCode)
			{
				return true;
			}

			errorMessage = "Failed to inspect generated build tree: " + buildDirectory.string() + ": " + errorCode.message();
			return false;
		}

		std::filesystem::directory_iterator iterator(buildDirectory, errorCode);
		const std::filesystem::directory_iterator end;
		while (!errorCode && iterator != end)
		{
			const std::filesystem::path path = iterator->path();
			iterator.increment(errorCode);
			if (path.lexically_normal() == dependencyCacheRoot.lexically_normal())
			{
				if (!RemoveDependencyBuildState(path, errorMessage))
				{
					return false;
				}
				continue;
			}

			if (!RemoveGeneratedPath(path, errorMessage))
			{
				return false;
			}
		}
		if (errorCode)
		{
			errorMessage = "Failed to enumerate generated build tree: " + buildDirectory.string() + ": " + errorCode.message();
			return false;
		}

		return true;
	}

	static bool RemoveProjectBuildOutputs(const SparkleTools::ToolOutputPaths& outputs, std::string& errorMessage)
	{
		const std::filesystem::path& projectsDirectory = outputs.ProjectArtifactRoot;
		std::error_code errorCode;
		if (!std::filesystem::is_directory(projectsDirectory, errorCode))
		{
			if (!errorCode)
			{
				return true;
			}

			errorMessage = "Failed to inspect project artifacts: " + projectsDirectory.string() + ": " + errorCode.message();
			return false;
		}

		std::filesystem::directory_iterator iterator(projectsDirectory, errorCode);
		const std::filesystem::directory_iterator end;
		while (!errorCode && iterator != end)
		{
			const std::filesystem::path projectDirectory = iterator->path();
			const bool bIsProjectDirectory = iterator->is_directory(errorCode);
			iterator.increment(errorCode);
			if (errorCode)
			{
				break;
			}
			if (!bIsProjectDirectory)
			{
				if (!RemoveGeneratedPath(projectDirectory, errorMessage))
				{
					return false;
				}
				continue;
			}

			const std::filesystem::path cookedDirectory = outputs.CookedProjectDirectory(projectDirectory.filename().string()).lexically_normal();
			std::filesystem::directory_iterator projectIterator(projectDirectory, errorCode);
			while (!errorCode && projectIterator != end)
			{
				const std::filesystem::path path = projectIterator->path();
				projectIterator.increment(errorCode);
				if (path.lexically_normal() == cookedDirectory)
				{
					continue;
				}
				if (!RemoveGeneratedPath(path, errorMessage))
				{
					return false;
				}
			}
		}
		if (errorCode)
		{
			errorMessage = "Failed to enumerate project artifacts: " + projectsDirectory.string() + ": " + errorCode.message();
			return false;
		}

		return true;
	}

	static bool RemoveCompiledDevelopmentArtifacts(const SparkleTools::ToolOutputPaths& outputs, std::string& errorMessage)
	{
		std::error_code errorCode;
		if (!std::filesystem::is_directory(outputs.ProductRoot, errorCode))
		{
			if (!errorCode)
			{
				return true;
			}

			errorMessage = "Failed to inspect development artifacts: " + outputs.ProductRoot.string() + ": " + errorCode.message();
			return false;
		}

		std::filesystem::directory_iterator iterator(outputs.ProductRoot, errorCode);
		const std::filesystem::directory_iterator end;
		while (!errorCode && iterator != end)
		{
			const std::filesystem::path path = iterator->path();
			iterator.increment(errorCode);
			if (path.lexically_normal() == outputs.ProjectArtifactRoot.lexically_normal())
			{
				continue;
			}
			if (!RemoveGeneratedPath(path, errorMessage))
			{
				return false;
			}
		}
		if (errorCode)
		{
			errorMessage = "Failed to enumerate development artifacts: " + outputs.ProductRoot.string() + ": " + errorCode.message();
			return false;
		}

		return RemoveProjectBuildOutputs(outputs, errorMessage);
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
