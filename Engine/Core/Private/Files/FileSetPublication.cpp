#include "PCH.h"

#include "Core/Public/Files/FileSetPublication.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Paths/PathUtils.h"

#include <Windows.h>

#include <format>
#include <memory>
#include <unordered_set>
#include <vector>

namespace Files
{
	static std::filesystem::path PublicationRoot(const std::filesystem::path& root)
	{
		if (root.empty())
		{
			throw Diagnostics::Error("Publication root is empty.");
		}
		std::error_code error;
		auto normalized = std::filesystem::weakly_canonical(std::filesystem::absolute(root), error);
		if (error || normalized == normalized.root_path())
		{
			throw Diagnostics::Error("Publication root must be a named output directory.");
		}
		return normalized;
	}

	class FileSetLock final
	{
	public:
		FileSetLock(const std::filesystem::path& root, bool bWriting)
		{
			std::error_code error;
			if (bWriting)
			{
				std::filesystem::create_directories(root, error);
			}
			if (error)
			{
				throw Diagnostics::Error(std::format("Could not create publication directory '{}': {}", root.string(), error.message()));
			}

			m_file = CreateFileW(
			    (root / ".publication-lock").c_str(),
			    GENERIC_READ | (bWriting ? GENERIC_WRITE : 0),
			    FILE_SHARE_READ | FILE_SHARE_WRITE,
			    nullptr,
			    bWriting ? OPEN_ALWAYS : OPEN_EXISTING,
			    FILE_ATTRIBUTE_NORMAL,
			    nullptr);

			if (m_file != INVALID_HANDLE_VALUE)
			{
				OVERLAPPED offset{};
				if (LockFileEx(m_file, LOCKFILE_FAIL_IMMEDIATELY | (bWriting ? LOCKFILE_EXCLUSIVE_LOCK : 0), 0, 1, 0, &offset))
				{
					return;
				}
				CloseHandle(m_file);
				m_file = INVALID_HANDLE_VALUE;
			}
			throw Diagnostics::Error(std::format("Publication directory '{}' is unavailable or in use; retry after its reader or writer settles.", root.string()));
		}

		~FileSetLock()
		{
			if (m_file != INVALID_HANDLE_VALUE)
			{
				CloseHandle(m_file);
			}
		}

		FileSetLock(const FileSetLock&) = delete;
		FileSetLock& operator=(const FileSetLock&) = delete;

	private:
		HANDLE m_file = INVALID_HANDLE_VALUE;
	};

	static std::vector<std::filesystem::path> PublicationRoots(std::span<const std::filesystem::path> roots)
	{
		std::vector<std::filesystem::path> result;
		for (const auto& root : roots)
		{
			const auto normalized = PublicationRoot(root);
			bool bContained = false;
			for (const auto& accepted : result)
			{
				bContained |= Paths::IsUnderRoot(normalized, accepted);
			}
			if (!bContained)
			{
				result.push_back(normalized);
			}
		}
		return result;
	}

	static void RequireCompletePublication(const std::filesystem::path& root)
	{
		std::error_code error;
		if (std::filesystem::exists(root / ".publication-incomplete", error) || error)
		{
			throw Diagnostics::Error(std::format("Incomplete publication at '{}'. Clean this generated output and cook again.", root.string()));
		}
	}

	struct FileSetReadScope::Implementation final
	{
		explicit Implementation(const std::filesystem::path& root) :
		    Root(PublicationRoot(root)),
		    Lock(Root, false)
		{
			RequireCompletePublication(Root);
		}

		std::filesystem::path Root;
		FileSetLock Lock;
	};

	FileSetReadScope::FileSetReadScope(const std::filesystem::path& root) :
	    m_implementation(std::make_unique<Implementation>(root))
	{
	}

	FileSetReadScope::~FileSetReadScope() = default;

	bool FileSetReadScope::Contains(const std::filesystem::path& path) const
	{
		std::error_code error;
		const auto normalized = std::filesystem::weakly_canonical(std::filesystem::absolute(path), error);
		return !error && normalized != m_implementation->Root && Paths::IsUnderRoot(normalized, m_implementation->Root);
	}

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

	struct FileSetPublisher::Implementation final
	{
		struct Replacement final
		{
			FilePublication File;
			bool bPreserved = false;
			bool bPublished = false;
		};

		explicit Implementation(std::span<const std::filesystem::path> roots) :
		    Roots(PublicationRoots(roots))
		{
			for (const auto& root : Roots)
			{
				Locks.push_back(std::make_unique<FileSetLock>(root, true));
			}
			RequireComplete();
		}

		void RequireComplete() const
		{
			for (const auto& root : Roots)
			{
				RequireCompletePublication(root);
			}
		}

		void RequirePublicationPaths(const FilePublication& file, std::unordered_set<std::wstring>& paths) const
		{
			for (const auto& path : {file.StagedPath, file.PublishedPath, BuildTemporaryPath(file.PublishedPath, ".previous-generation")})
			{
				bool bContained = false;
				for (const auto& root : Roots)
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

		std::vector<Replacement> PlanReplacements(std::span<const FilePublication> files) const
		{
			std::vector<Replacement> replacements;
			replacements.reserve(files.size());
			std::unordered_set<std::wstring> paths;
			for (const FilePublication& file : files)
			{
				if (file.StagedPath.empty() || file.PublishedPath.empty())
				{
					throw Diagnostics::Error("Publication paths must not be empty.");
				}
				Replacement& replacement = replacements.emplace_back();
				replacement.File = {Paths::Normalize(file.StagedPath), Paths::Normalize(file.PublishedPath)};
				RequirePublicationPaths(replacement.File, paths);
				RequirePublicationInputs(replacement.File);
			}
			return replacements;
		}

		void ClearMarkers() const
		{
			for (const auto& root : Roots)
			{
				RemovePublicationFile(root / ".publication-incomplete");
			}
		}

		void MarkIncomplete() const
		{
			for (const auto& root : Roots)
			{
				std::string error;
				if (!TryWriteNewFileDurable(root / ".publication-incomplete", {}, error))
				{
					throw Diagnostics::Error(std::format("Could not mark publication at '{}': {}", root.string(), error));
				}
			}
		}

		static void ReplaceFiles(std::span<Replacement> replacements)
		{
			for (Replacement& replacement : replacements)
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

		static void DiscardBackups(std::span<const Replacement> replacements)
		{
			for (const Replacement& replacement : replacements)
			{
				if (replacement.bPreserved)
				{
					RemovePublicationFile(BuildTemporaryPath(replacement.File.PublishedPath, ".previous-generation"));
				}
			}
		}

		static void Rollback(std::span<const Replacement> replacements)
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

		bool Publish(std::span<const FilePublication> files, std::string& outErrorMessage)
		{
			std::vector<Replacement> replacements;
			bool bStarted = false;
			bool bCommitted = false;
			try
			{
				RequireComplete();
				replacements = PlanReplacements(files);
				bStarted = !files.empty();
				if (bStarted)
				{
					MarkIncomplete();
				}
				ReplaceFiles(replacements);

				bCommitted = true;
				DiscardBackups(replacements);
				ClearMarkers();
				outErrorMessage.clear();
				return true;
			}
			catch (const Diagnostics::Error& error)
			{
				outErrorMessage = error.what();
				if (bStarted && !bCommitted)
				{
					try
					{
						Rollback(replacements);
						ClearMarkers();
						return false;
					}
					catch (const Diagnostics::Error& rollback)
					{
						outErrorMessage += std::format(" Rollback failed: {}", rollback.what());
					}
				}
				if (bStarted)
				{
					outErrorMessage += " Clean generated output and cook again.";
				}
				return false;
			}
		}

		std::vector<std::filesystem::path> Roots;
		std::vector<std::unique_ptr<FileSetLock>> Locks;
	};

	FileSetPublisher::FileSetPublisher(std::span<const std::filesystem::path> roots) :
	    m_implementation(std::make_unique<Implementation>(roots))
	{
	}

	FileSetPublisher::~FileSetPublisher() = default;

	bool FileSetPublisher::Publish(std::span<const FilePublication> files, std::string& outErrorMessage)
	{
		return m_implementation->Publish(files, outErrorMessage);
	}
}
