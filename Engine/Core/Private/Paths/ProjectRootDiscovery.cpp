#include "PCH.h"

#include "Paths/FileSystemDiscovery.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Paths/PathFormatting.h"
#include "Core/Public/Paths/PathUtils.h"
#include "Core/Public/Strings/StringUtils.h"

#include <system_error>

namespace Filesystem
{
	std::string GetExecutableStem()
	{
		return Filesystem::GetExecutablePath().stem().string();
	}

	std::string InferProjectNameFromExecutableStem(std::string executableStem)
	{
		const bool bExecutableHasEditorSuffix = executableStem.size() > std::string_view("Editor").size() && PathFormatting::EndsWithIgnoreCase(executableStem, "Editor");

		if (bExecutableHasEditorSuffix)
		{
			executableStem.resize(executableStem.size() - std::string_view("Editor").size());
		}
		else if (executableStem.size() > std::string_view("Game").size() && PathFormatting::EndsWithIgnoreCase(executableStem, "Game"))
		{
			executableStem.resize(executableStem.size() - std::string_view("Game").size());
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
		const std::string executableStem = InferProjectNameFromExecutableStem(GetExecutableStem());
		if (executableStem.empty())
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

			if (Strings::EqualsIgnoreCase(projectRoot.filename().string(), executableStem))
			{
				return Paths::Normalize(projectRoot);
			}
		}

		return std::nullopt;
	}

}
