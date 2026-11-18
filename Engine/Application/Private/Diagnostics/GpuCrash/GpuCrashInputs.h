#pragma once

#include "Startup/ApplicationGpuCrashStartup.h"
#include "RHI/Public/Core/RhiBackendSelection.h"

#include <nlohmann/json.hpp>

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
	const nlohmann::json& GetIdentity() const noexcept;

private:
	std::array<HANDLE, 2> m_cookedFiles{INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE};
	nlohmann::json m_identity;
};
