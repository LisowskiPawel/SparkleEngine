#include "PCH.h"

#include "Core/Public/Paths/FileSystemPaths.h"

#if defined(_WIN32)
  #define WIN32_LEAN_AND_MEAN
  #define NOMINMAX
  #include <Windows.h>
#endif

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

}
