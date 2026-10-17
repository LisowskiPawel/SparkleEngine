#pragma once

#include "ApplicationGpuCrashStartup.h"
#include "RHI/Public/Core/RhiBackendSelection.h"

#include <Windows.h>
#include <array>
#include <string>

std::filesystem::path ResolveAftermathLibraryPath();

class GpuCrashInputs final
{
public:
	GpuCrashInputs(const GpuCrashStartup& startup, ERhiBackendApi api);
	~GpuCrashInputs();
	GpuCrashInputs(const GpuCrashInputs&) = delete;
	GpuCrashInputs& operator=(const GpuCrashInputs&) = delete;
	const std::string& GetIdentity() const noexcept;

private:
	std::array<HANDLE, 2> m_cookedFiles{INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE};
	std::string m_identity;
};
