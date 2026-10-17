#pragma once

#include "Core/Public/Process/ChildProcess.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>

namespace SparkleLauncher
{
	struct GpuCrashIncident;
	struct LevelRunOperationPlan;

	struct GpuCrashReadinessExpectation final
	{
		std::filesystem::path ExecutablePath;
		std::filesystem::path ShaderExportPath;
		std::string SessionId;
		std::string Profile;
		std::string GraphicsApi;
	};

	bool PrepareGpuCrashLaunch(LevelRunOperationPlan& plan);
	std::filesystem::path ReadGpuCrashAnnouncement(std::string_view output);
	Process::ChildReadinessDecision VerifyGpuCrashReadiness(const GpuCrashReadinessExpectation& expected, std::uint32_t childPid, std::string_view output, std::string& error);
	std::string ReadGpuCrashState(const std::filesystem::path& manifestPath, std::string_view sessionId, std::uint32_t childPid, std::string* verifiedSnapshot = nullptr);
	std::string ReadGpuCrashCollectionState(const GpuCrashIncident& incident);
}
