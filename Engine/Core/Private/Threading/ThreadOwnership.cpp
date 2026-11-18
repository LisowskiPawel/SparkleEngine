#include "PCH.h"

#include "Core/Public/Threading/ThreadOwnership.h"

#include "Core/Public/Diagnostics/Logger.h"
#include "Core/Public/Diagnostics/Verify.h"

#include <utility>

#if defined(_WIN32)
  #define WIN32_LEAN_AND_MEAN
  #define NOMINMAX
  #include <windows.h>
#endif

namespace Threading
{

	SPARKLE_DEFINE_LOG_CATEGORY_STATIC(LogThreadOwnership, "Threading.Ownership");

	void SetCurrentThreadRole(std::string_view role) noexcept
	{
#if defined(_WIN32)
		try
		{
			const std::wstring wideRole(role.begin(), role.end());
			(void) ::SetThreadDescription(::GetCurrentThread(), wideRole.c_str());
		}
		catch (...)
		{
			// A diagnostic label must never alter program control flow.
		}
#else
		(void) role;
#endif
	}

	OwnerThread::OwnerThread(std::string ownerDescription) noexcept :
	    m_ownerDescription("Thread-affinity violation: " + std::move(ownerDescription))
	{
	}

	void OwnerThread::AssertAccess(std::source_location location) const noexcept
	{
		if (std::this_thread::get_id() == m_thread)
		{
			return;
		}

		Diagnostics::Fatal(LogThreadOwnership, location.file_name(), location.line(), m_ownerDescription);
	}
}
