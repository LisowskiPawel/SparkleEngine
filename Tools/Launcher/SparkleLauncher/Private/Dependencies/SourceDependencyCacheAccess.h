#pragma once

#include <Windows.h>

#include <cstdint>
#include <filesystem>
#include <string>

namespace SparkleLauncher
{
	enum class SourceDependencyCacheAccessMode : std::uint8_t
	{
		Read,
		Write
	};

	class SourceDependencyCacheAccess final
	{
	public:
		SourceDependencyCacheAccess(const std::filesystem::path& workspaceRoot, SourceDependencyCacheAccessMode mode);
		~SourceDependencyCacheAccess();
		SourceDependencyCacheAccess(const SourceDependencyCacheAccess&) = delete;
		SourceDependencyCacheAccess& operator=(const SourceDependencyCacheAccess&) = delete;

		bool IsAcquired() const { return m_file != INVALID_HANDLE_VALUE; }

		const std::string& GetErrorMessage() const { return m_errorMessage; }

	private:
		HANDLE m_file = INVALID_HANDLE_VALUE;
		std::string m_errorMessage;
	};
}
