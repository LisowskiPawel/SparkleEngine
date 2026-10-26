#include "PCH.h"

#include "AssetPathState.h"

#include <cstring>
#include <memory>
#include <string>
#include <system_error>

namespace Filesystem::Private
{
	static void ValidatePath(const std::shared_ptr<spdlog::logger>& logger, const char* label, const std::filesystem::path& path, bool required)
	{
		constexpr std::size_t LabelWidth = 24;
		const std::size_t labelLength = std::strlen(label);
		const std::string paddedLabel = std::string(label) + std::string(LabelWidth - labelLength, ' ');
		if (path.empty())
		{
			if (required)
			{
				Diagnostics::Fatal(logger, __FILE__, __LINE__, "[MISSING]  " + paddedLabel + ": (not configured)");
			}
			return;
		}

		std::error_code errorCode;
		if (std::filesystem::exists(path, errorCode))
		{
			return;
		}

		if (required)
		{
			Diagnostics::Fatal(logger, __FILE__, __LINE__, "[MISSING]  " + paddedLabel + ": " + path.string());
		}
		else
		{
			SPDLOG_LOGGER_WARN(logger, "[MISSING]  {}: {}", paddedLabel, path.string());
		}
	}

	void ValidateConfiguredPaths(const AssetPathState& state)
	{
		SPARKLE_DEFINE_LOG_CATEGORY_STATIC(logger, "Core");
		ValidatePath(logger, "Working Directory", state.workingDirectory, true);
		ValidatePath(logger, "Executable Directory", state.executableDirectory, true);
		ValidatePath(logger, "Workspace", state.workspacePath, true);
		ValidatePath(logger, "Logs Root", state.productUserStatePaths.LogsRoot, true);
		ValidatePath(logger, "Cooked Asset Root", state.cookedAssetRootPath, true);
		ValidatePath(logger, "Cooked Shader Root", state.cookedShaderRootPath, true);
		ValidatePath(logger, "Cooked Skeleton Root", state.cookedSkeletonRootPath, true);
		ValidatePath(logger, "Cooked Animation Root", state.cookedAnimationRootPath, true);
		ValidatePath(logger, "Engine", state.enginePath, true);
		ValidatePath(logger, "Engine Assets", state.engineAssetsPath, true);
		ValidatePath(logger, "Project", state.projectPath, state.installedProduct);
		ValidatePath(logger, "Project Assets", state.projectAssetsPath, false);
		ValidatePath(logger, "Shader Symbols Output", state.shaderSymbolsOutputPath, false);
	}

}
