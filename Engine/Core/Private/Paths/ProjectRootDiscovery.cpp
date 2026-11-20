#include "PCH.h"

#include "Paths/FileSystemDiscovery.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Paths/PathFormatting.h"
#include "Core/Public/Paths/PathUtils.h"
#include "Core/Public/Strings/StringUtils.h"

#include <system_error>
#include <utility>

namespace Filesystem
{
	std::string GetExecutableStem()
	{
		return Filesystem::GetExecutablePath().stem().string();
	}

	std::string InferProjectNameFromExecutableStem(std::string executableStem)
	{
		constexpr std::string_view editorSuffix = "Editor";
		constexpr std::string_view gameSuffix = "Game";
		const bool bExecutableHasEditorSuffix = executableStem.size() > editorSuffix.size() && PathFormatting::EndsWithIgnoreCase(executableStem, editorSuffix);

		if (bExecutableHasEditorSuffix)
		{
			executableStem.resize(executableStem.size() - editorSuffix.size());
		}
		else if (executableStem.size() > gameSuffix.size() && PathFormatting::EndsWithIgnoreCase(executableStem, gameSuffix))
		{
			executableStem.resize(executableStem.size() - gameSuffix.size());
		}

		return executableStem;
	}

	std::optional<std::filesystem::path> DiscoverWorkspaceProjectRoot()
	{
		const auto workspaceRoot = Filesystem::DiscoverWorkspaceRoot();
		if (!workspaceRoot)
		{
			return std::nullopt;
		}

		const std::filesystem::path projectsRoot = *workspaceRoot / "Projects";
		std::string executableStem = GetExecutableStem();
		const std::string projectName = InferProjectNameFromExecutableStem(std::move(executableStem));
		if (projectName.empty())
		{
			return std::nullopt;
		}

		std::error_code ec;
		for (const auto& entry : std::filesystem::directory_iterator(projectsRoot, ec))
		{
			if (ec)
			{
				break;
			}

			if (!entry.is_directory(ec))
			{
				ec.clear();
				continue;
			}

			const std::filesystem::path& projectRoot = entry.path();
			if (!std::filesystem::exists(projectRoot / Filesystem::kProjectMarker, ec))
			{
				ec.clear();
				continue;
			}

			const std::string directoryName = projectRoot.filename().string();
			if (Strings::EqualsIgnoreCase(directoryName, projectName))
			{
				return Paths::Normalize(projectRoot);
			}
		}

		return std::nullopt;
	}

}
