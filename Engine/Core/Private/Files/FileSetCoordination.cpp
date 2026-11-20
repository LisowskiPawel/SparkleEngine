#include "PCH.h"

#include "Files/FileSetCoordination.h"
#include "Core/Public/Files/FileSetPublication.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Paths/PathUtils.h"

#include <format>
#include <unordered_set>
#include <utility>

namespace Files
{
	std::filesystem::path PublicationRoot(const std::filesystem::path& root)
	{
		if (root.empty())
		{
			throw Diagnostics::Error("Publication root is empty.");
		}
		std::error_code error;
		const auto absoluteRoot = std::filesystem::absolute(root);
		auto normalized = std::filesystem::weakly_canonical(absoluteRoot, error);
		if (error || normalized == normalized.root_path())
		{
			throw Diagnostics::Error("Publication root must be a named output directory.");
		}
		return normalized;
	}

	FileSetLock::FileSetLock(const std::filesystem::path& root, bool bWriting)
	{
		std::error_code error;
		if (bWriting)
		{
			std::filesystem::create_directories(root, error);
		}
		if (error)
		{
			const std::string message = std::format("Could not create publication directory '{}': {}", root.string(), error.message());
			throw Diagnostics::Error(message);
		}

		const auto lockPath = root / ".publication-lock";

		m_file = CreateFileW(
		    lockPath.c_str(),
		    GENERIC_READ | (bWriting ? GENERIC_WRITE : 0),
		    FILE_SHARE_READ | FILE_SHARE_WRITE,
		    nullptr,
		    bWriting ? OPEN_ALWAYS : OPEN_EXISTING,
		    FILE_ATTRIBUTE_NORMAL,
		    nullptr);

		if (m_file != INVALID_HANDLE_VALUE)
		{
			OVERLAPPED offset{};
			const DWORD lockFlags = LOCKFILE_FAIL_IMMEDIATELY | (bWriting ? LOCKFILE_EXCLUSIVE_LOCK : 0);
			if (LockFileEx(m_file, lockFlags, 0, 1, 0, &offset))
			{
				return;
			}
			CloseHandle(m_file);
			m_file = INVALID_HANDLE_VALUE;
		}
		const std::string message = std::format("Publication directory '{}' is unavailable or in use; retry after its reader or writer settles.", root.string());
		throw Diagnostics::Error(message);
	}

	FileSetLock::~FileSetLock()
	{
		if (m_file != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_file);
		}
	}

	std::vector<std::filesystem::path> PublicationRoots(std::span<const std::filesystem::path> roots)
	{
		std::vector<std::filesystem::path> result;
		std::unordered_set<std::wstring> keys;
		for (const auto& root : roots)
		{
			const auto normalized = PublicationRoot(root);
			auto key = Paths::MakePathKey(normalized);
			const bool bInserted = keys.insert(std::move(key)).second;
			if (bInserted)
			{
				result.push_back(normalized);
			}
		}
		return result;
	}

	void RequireCompletePublication(const std::filesystem::path& root)
	{
		std::error_code error;
		if (std::filesystem::exists(root / ".publication-incomplete", error) || error)
		{
			const std::string message = std::format("Incomplete publication at '{}'. Clean this generated output and cook again.", root.string());
			throw Diagnostics::Error(message);
		}
	}

	void RequireCompletePublications(std::span<const std::filesystem::path> roots)
	{
		for (const auto& root : roots)
		{
			RequireCompletePublication(root);
		}
	}

	void ClearPublicationMarkers(std::span<const std::filesystem::path> roots)
	{
		for (const auto& root : roots)
		{
			const auto marker = root / ".publication-incomplete";
			std::error_code error;
			std::filesystem::remove(marker, error);
			if (error)
			{
				const std::string message = std::format("Could not remove publication file '{}': {}", marker.string(), error.message());
				throw Diagnostics::Error(message);
			}
		}
	}

	void MarkPublicationsIncomplete(std::span<const std::filesystem::path> roots)
	{
		for (const auto& root : roots)
		{
			std::string error;
			if (!TryWriteNewFileDurable(root / ".publication-incomplete", {}, error))
			{
				const std::string message = std::format("Could not mark publication at '{}': {}", root.string(), error);
				throw Diagnostics::Error(message);
			}
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
		const auto relativePath = Paths::TryMakeRelativeUnderRoot(path, m_implementation->Root);
		return relativePath && *relativePath != ".";
	}

}
