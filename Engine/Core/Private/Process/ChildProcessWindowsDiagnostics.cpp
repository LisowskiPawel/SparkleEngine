#include "Process/ChildProcessWindows.h"

#if defined(_WIN32)
namespace Process
{
	std::string WideToUtf8(std::wstring_view text)
	{
		if (text.empty())
		{
			return {};
		}

		const int length = WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
		if (length <= 0)
		{
			return "Unknown Windows error";
		}

		std::string result(static_cast<std::size_t>(length), '\0');
		WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), result.data(), length, nullptr, nullptr);
		return result;
	}

	std::string FormatWindowsProcessError(DWORD code)
	{
		LPWSTR raw = nullptr;

		const DWORD length = FormatMessageW(
		    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		    nullptr,
		    code,
		    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		    reinterpret_cast<LPWSTR>(&raw),
		    0,
		    nullptr);

		std::string message = length != 0 && raw != nullptr ? WideToUtf8(std::wstring_view(raw, length)) : "Unknown Windows error";
		if (raw != nullptr)
		{
			LocalFree(raw);
		}

		const std::size_t lastContent = message.find_last_not_of(" \t\r\n");
		message.erase(lastContent == std::string::npos ? 0 : lastContent + 1);
		return message;
	}

}
#endif
