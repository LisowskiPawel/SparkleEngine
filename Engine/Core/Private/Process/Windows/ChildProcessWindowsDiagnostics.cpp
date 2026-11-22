#include "Process/Windows/ChildProcessWindows.h"
#include "Strings/StringUtils.h"

#include <memory>

#if defined(_WIN32)
namespace Process
{
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

		const std::unique_ptr<wchar_t, decltype(&LocalFree)> buffer(raw, &LocalFree);
		const std::wstring_view systemMessage = buffer != nullptr ? std::wstring_view(buffer.get(), length) : std::wstring_view{};
		std::string message = systemMessage.empty() ? "Unknown Windows error" : Strings::ToNarrow(systemMessage);

		const std::size_t lastContent = message.find_last_not_of(" \t\r\n");
		message.erase(lastContent == std::string::npos ? 0 : lastContent + 1);
		return message;
	}

}
#endif
