#include "PCH.h"

#include "Files/FileSetReplacement.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Paths/PathUtils.h"

#include <Windows.h>

#include <format>
#include <unordered_set>

namespace Files
{
	static void RemovePublicationFile(const std::filesystem::path& path)
	{
		std::error_code error;
		std::filesystem::remove(path, error);
		if (error)
		{
			throw Diagnostics::Error(std::format("Could not remove publication file '{}': {}", path.string(), error.message()));
		}
	}

	static void MovePublishedFile(const std::filesystem::path& source, const std::filesystem::path& destination)
	{
		if (!MoveFileExW(source.c_str(), destination.c_str(), MOVEFILE_REPLACE_EXISTING))
		{
			throw Diagnostics::Error(std::format("Could not replace publication file '{}' with '{}': {}", destination.string(), source.string(), GetLastError()));
		}
	}

	static void RequirePublicationPaths(const FilePublication& file, std::span<const std::filesystem::path> roots, std::unordered_set<std::wstring>& paths)
	{
		for (const auto& path : {file.StagedPath, file.PublishedPath, BuildTemporaryPath(file.PublishedPath, ".previous-generation")})
		{
			bool bContained = false;
			for (const auto& root : roots)
			{
				bContained |= path != root && Paths::IsUnderRoot(path, root);
			}
			if (!bContained)
			{
				throw Diagnostics::Error(std::format("Publication path '{}' escapes its output directories.", path.string()));
			}
			if (path.filename().wstring().starts_with(L".publication-") || !paths.insert(Paths::MakePathKey(path)).second)
			{
				throw Diagnostics::Error("Publication paths overlap each other or coordination files.");
			}
		}
	}

	static void RequirePublicationInputs(const FilePublication& file)
	{
		std::error_code error;
		if (!std::filesystem::is_regular_file(file.StagedPath, error) || error)
		{
			throw Diagnostics::Error("Publication staging input is missing or is not a regular file.");
		}
		const bool bPublishedExists = std::filesystem::exists(file.PublishedPath, error);
		if (error || (bPublishedExists && !std::filesystem::is_regular_file(file.PublishedPath, error)) || error)
		{
			throw Diagnostics::Error("Publication destination is not a regular file.");
		}
		if (std::filesystem::exists(BuildTemporaryPath(file.PublishedPath, ".previous-generation"), error) || error)
		{
			throw Diagnostics::Error("Publication has an orphaned backup. Clean generated output and cook again.");
		}
	}

	std::vector<FileSetReplacement> PlanFileSetReplacements(std::span<const FilePublication> files, std::span<const std::filesystem::path> roots)
	{
		std::vector<FileSetReplacement> replacements;
		replacements.reserve(files.size());
		std::unordered_set<std::wstring> paths;
		for (const FilePublication& file : files)
		{
			if (file.StagedPath.empty() || file.PublishedPath.empty())
			{
				throw Diagnostics::Error("Publication paths must not be empty.");
			}
			FileSetReplacement& replacement = replacements.emplace_back();
			replacement.File = {Paths::Normalize(file.StagedPath), Paths::Normalize(file.PublishedPath)};
			RequirePublicationPaths(replacement.File, roots, paths);
			RequirePublicationInputs(replacement.File);
		}
		return replacements;
	}

	void ReplaceFileSet(std::span<FileSetReplacement> replacements)
	{
		for (FileSetReplacement& replacement : replacements)
		{
			std::error_code error;
			const bool bExists = std::filesystem::exists(replacement.File.PublishedPath, error);
			if (error)
			{
				throw Diagnostics::Error("Could not inspect the previous publication output.");
			}
			if (bExists)
			{
				MovePublishedFile(replacement.File.PublishedPath, BuildTemporaryPath(replacement.File.PublishedPath, ".previous-generation"));
				replacement.bPreserved = true;
			}
			MovePublishedFile(replacement.File.StagedPath, replacement.File.PublishedPath);
			replacement.bPublished = true;
		}
	}

	void DiscardFileSetBackups(std::span<const FileSetReplacement> replacements)
	{
		for (const FileSetReplacement& replacement : replacements)
		{
			if (replacement.bPreserved)
			{
				RemovePublicationFile(BuildTemporaryPath(replacement.File.PublishedPath, ".previous-generation"));
			}
		}
	}

	void RollbackFileSet(std::span<const FileSetReplacement> replacements)
	{
		for (auto replacement = replacements.rbegin(); replacement != replacements.rend(); ++replacement)
		{
			if (replacement->bPreserved)
			{
				MovePublishedFile(BuildTemporaryPath(replacement->File.PublishedPath, ".previous-generation"), replacement->File.PublishedPath);
			}
			else if (replacement->bPublished)
			{
				RemovePublicationFile(replacement->File.PublishedPath);
			}
			RemovePublicationFile(replacement->File.StagedPath);
		}
	}

}
