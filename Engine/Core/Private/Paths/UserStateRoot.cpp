#include "PCH.h"

#include "Paths/UserStatePaths.h"

#include <cstdlib>
#include <stdexcept>
#include <system_error>

#if defined(_WIN32)
  #define WIN32_LEAN_AND_MEAN
  #define NOMINMAX
  #include <Windows.h>
  #include <ShlObj.h>
#endif

namespace Filesystem
{
	static std::filesystem::path DiscoverPlatformLocalStateRoot()
	{
#if defined(_WIN32)
		PWSTR knownFolderPath = nullptr;
		if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DEFAULT, nullptr, &knownFolderPath)))
		{
			const std::filesystem::path result(knownFolderPath);
			CoTaskMemFree(knownFolderPath);
			return result;
		}
		throw std::runtime_error("Cannot resolve Windows LocalAppData for Sparkle per-user state.");
#else
		if (const char* stateHome = std::getenv("XDG_STATE_HOME"); stateHome != nullptr && *stateHome != '\0')
		{
			return std::filesystem::path(stateHome);
		}
		if (const char* home = std::getenv("HOME"); home != nullptr && *home != '\0')
		{
			return std::filesystem::path(home) / ".local" / "state";
		}
#endif
		std::error_code errorCode;
		const std::filesystem::path temporaryRoot = std::filesystem::temp_directory_path(errorCode);
		if (!errorCode)
		{
			return temporaryRoot;
		}
		errorCode.clear();
		return std::filesystem::current_path(errorCode);
	}

	std::filesystem::path ResolveSparkleUserStateRoot()
	{
		return DiscoverPlatformLocalStateRoot() / "SparkleEngine";
	}

}
