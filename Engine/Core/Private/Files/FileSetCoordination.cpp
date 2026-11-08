#include "PCH.h"

#include "Files/FileSetCoordination.h"
#include "Core/Public/Files/FileSetPublication.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Paths/PathUtils.h"

#include <format>

namespace Files
{
	std::filesystem::path PublicationRoot(const std::filesystem::path& root)
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

	FileSetLock::FileSetLock(const std::filesystem::path& root, bool bWriting)
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

	void RequireCompletePublication(const std::filesystem::path& root)
	{
		std::error_code error;
		if (std::filesystem::exists(root / ".publication-incomplete", error) || error)
		{
			throw Diagnostics::Error(std::format("Incomplete publication at '{}'. Clean this generated output and cook again.", root.string()));
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
				throw Diagnostics::Error(std::format("Could not remove publication file '{}': {}", marker.string(), error.message()));
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
				throw Diagnostics::Error(std::format("Could not mark publication at '{}': {}", root.string(), error));
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
		std::error_code error;
		const auto normalized = std::filesystem::weakly_canonical(std::filesystem::absolute(path), error);
		return !error && normalized != m_implementation->Root && Paths::IsUnderRoot(normalized, m_implementation->Root);
	}

}
