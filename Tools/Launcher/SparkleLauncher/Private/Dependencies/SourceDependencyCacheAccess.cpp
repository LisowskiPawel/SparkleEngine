#include "Dependencies/SourceDependencyCacheAccess.h"

#include <system_error>

namespace SparkleLauncher
{
	SourceDependencyCacheAccess::SourceDependencyCacheAccess(const std::filesystem::path& workspaceRoot, SourceDependencyCacheAccessMode mode)
	{
		const auto lockPath = workspaceRoot / SPARKLE_SOURCE_CACHE_LOCK_NAME;
		const bool bWriting = mode == SourceDependencyCacheAccessMode::Write;
		const DWORD access = bWriting ? GENERIC_READ | GENERIC_WRITE : GENERIC_READ;
		const DWORD sharing = bWriting ? 0 : FILE_SHARE_READ;
		m_file = CreateFileW(lockPath.c_str(), access, sharing, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (m_file == INVALID_HANDLE_VALUE)
		{
			const std::error_code error(static_cast<int>(GetLastError()), std::system_category());
			m_errorMessage = "Source dependency cache is in use or unavailable. Retry after builds, configure and Clean settle. " + error.message();
		}
	}

	SourceDependencyCacheAccess::~SourceDependencyCacheAccess()
	{
		if (m_file != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_file);
		}
	}
}
