#include "PCH.h"

#include "Paths/FileSystemDiscovery.h"

#include "Core/Public/FileSystemUtils.h"
#include "Core/Public/Paths/PackagePaths.h"
#include "Core/Public/Paths/PathFormatting.h"
#include "Core/Public/Paths/PathUtils.h"
#include "Core/Public/Strings/StringUtils.h"

#include <string_view>
#include <system_error>
#include <utility>

#if defined(_WIN32)
  #define WIN32_LEAN_AND_MEAN
  #define NOMINMAX
  #include <Windows.h>
#endif

namespace Filesystem::Private
{
	std::string GetExecutableStem()
	{
		return Filesystem::GetExecutablePath().stem().string();
	}

	std::string InferProjectNameFromExecutableStem(std::string executableStem)
	{
		if (executableStem.size() > std::string_view("Editor").size() && PathFormatting::EndsWithIgnoreCase(executableStem, "Editor"))
		{
			executableStem.resize(executableStem.size() - std::string_view("Editor").size());
		}
		else if (executableStem.size() > std::string_view("Game").size() && PathFormatting::EndsWithIgnoreCase(executableStem, "Game"))
		{
			executableStem.resize(executableStem.size() - std::string_view("Game").size());
		}

		if (executableStem.empty())
		{
			return {};
		}

		return executableStem;
	}

	static std::optional<std::filesystem::path> DiscoverWorkspaceProjectRoot()
	{
		const auto workspaceRoot = Filesystem::DiscoverWorkspaceRoot();
		if (!workspaceRoot)
		{
			return std::nullopt;
		}

		const std::filesystem::path projectsRoot = *workspaceRoot / "Projects";
		std::error_code ec;
		if (!std::filesystem::exists(projectsRoot, ec) || ec)
		{
			return std::nullopt;
		}

		const std::string executableStem = InferProjectNameFromExecutableStem(GetExecutableStem());
		if (executableStem.empty())
		{
			return std::nullopt;
		}

		for (const auto& entry : std::filesystem::directory_iterator(projectsRoot, ec))
		{
			if (ec)
			{
				break;
			}

			if (!entry.is_directory(ec) || ec)
			{
				ec.clear();
				continue;
			}

			const std::filesystem::path& projectRoot = entry.path();
			if (!std::filesystem::exists(projectRoot / Filesystem::kProjectMarker, ec) || ec)
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

namespace Filesystem
{
	std::filesystem::path GetExecutablePath()
	{
#if defined(_WIN32)
		for (std::size_t capacity = 512u; capacity <= 32768u; capacity *= 2u)
		{
			std::wstring buffer(capacity, L'\0');
			const DWORD length = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
			if (length == 0u)
			{
				return {};
			}

			if (length < buffer.size())
			{
				buffer.resize(length);
				return std::filesystem::path(buffer);
			}
		}
#endif
		return {};
	}

	std::filesystem::path GetExecutableDirectory()
	{
		const std::filesystem::path executablePath = GetExecutablePath();
		return executablePath.empty() ? std::filesystem::current_path() : executablePath.parent_path();
	}

	std::optional<std::filesystem::path> FindAncestorWithMarker(const std::filesystem::path& startDirectory, std::string_view markerFileName, std::uint32_t maxDepth)
	{
		if (startDirectory.empty() || markerFileName.empty())
		{
			return std::nullopt;
		}

		std::error_code errorCode;
		std::filesystem::path currentDirectory = std::filesystem::weakly_canonical(startDirectory, errorCode);
		if (errorCode)
		{
			currentDirectory = startDirectory;
		}

		for (std::uint32_t depth = 0; depth < maxDepth && !currentDirectory.empty(); ++depth)
		{
			if (std::filesystem::exists(currentDirectory / markerFileName, errorCode))
			{
				return currentDirectory;
			}

			std::filesystem::path parentDirectory = currentDirectory.parent_path();
			if (parentDirectory == currentDirectory)
			{
				break;
			}
			currentDirectory = std::move(parentDirectory);
		}
		return std::nullopt;
	}

	static std::optional<std::filesystem::path> FindProcessAncestorWithMarker(std::string_view marker)
	{
		if (const auto executableRoot = FindAncestorWithMarker(GetExecutableDirectory(), marker))
		{
			return Paths::Normalize(*executableRoot);
		}

		std::error_code errorCode;
		if (const auto workingRoot = FindAncestorWithMarker(std::filesystem::current_path(errorCode), marker); workingRoot && !errorCode)
		{
			return Paths::Normalize(*workingRoot);
		}
		return std::nullopt;
	}

	std::optional<std::filesystem::path> DiscoverWorkspaceRoot()
	{
		return FindProcessAncestorWithMarker(kWorkspaceMarker);
	}

	std::optional<std::filesystem::path> DiscoverEngineRoot()
	{
		if (const auto processRoot = FindProcessAncestorWithMarker(kEngineMarker))
		{
			return processRoot;
		}
		std::error_code errorCode;
		if (const auto workspaceRoot = DiscoverWorkspaceRoot())
		{
			const std::filesystem::path enginePath = *workspaceRoot / "engine";
			if (std::filesystem::exists(enginePath / kEngineMarker, errorCode))
			{
				return Paths::Normalize(enginePath);
			}
		}
		return std::nullopt;
	}

	std::optional<std::filesystem::path> DiscoverProjectRoot()
	{
		if (const auto package = DiscoverPackageRoot())
		{
			return ResolvePackageSourceRoot(*package);
		}
		if (const auto processRoot = FindProcessAncestorWithMarker(kProjectMarker))
		{
			return processRoot;
		}
		if (const auto workspaceProjectRoot = Private::DiscoverWorkspaceProjectRoot())
		{
			return Paths::Normalize(*workspaceProjectRoot);
		}

		return std::nullopt;
	}

	std::filesystem::path ResolveWorkspaceRootPath()
	{
		if (const auto workspaceRoot = DiscoverWorkspaceRoot())
		{
			return Paths::Normalize(*workspaceRoot);
		}
		if (const auto engineRoot = DiscoverEngineRoot())
		{
			return Paths::Normalize(engineRoot->parent_path());
		}

		std::error_code errorCode;
		const std::filesystem::path workingDirectory = std::filesystem::current_path(errorCode);
		return !workingDirectory.empty() && !errorCode ? Paths::Normalize(workingDirectory) : Paths::Normalize(GetExecutableDirectory());
	}

}
