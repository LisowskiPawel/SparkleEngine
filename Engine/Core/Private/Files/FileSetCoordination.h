#pragma once

#include <Windows.h>

#include <filesystem>
#include <span>
#include <vector>

namespace Files
{
	class FileSetLock final
	{
	public:
		FileSetLock(const std::filesystem::path& root, bool bWriting);
		~FileSetLock();
		FileSetLock(const FileSetLock&) = delete;
		FileSetLock& operator=(const FileSetLock&) = delete;

	private:
		HANDLE m_file = INVALID_HANDLE_VALUE;
	};

	std::filesystem::path PublicationRoot(const std::filesystem::path& root);
	std::vector<std::filesystem::path> PublicationRoots(std::span<const std::filesystem::path> roots);
	void RequireCompletePublication(const std::filesystem::path& root);
	void RequireCompletePublications(std::span<const std::filesystem::path> roots);
	void MarkPublicationsIncomplete(std::span<const std::filesystem::path> roots);
	void ClearPublicationMarkers(std::span<const std::filesystem::path> roots);
}
