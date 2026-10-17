#pragma once

#include "Core/Public/CoreAPI.h"

#include <cstdint>
#include <filesystem>
#include <functional>
#include <optional>
#include <stop_token>
#include <string>
#include <string_view>
#include <vector>

namespace Process
{
	enum class ChildProcessStartFailure : std::uint8_t
	{
		None,
		ExecutableNotFound,
		AccessDenied,
		BlockedByPolicy,
		OperatingSystemError
	};

	enum class ChildReadinessDecision : std::uint8_t
	{
		Ready,
		Rejected,
		ReleaseWithoutReadiness
	};

	struct EnvironmentOverride final
	{
		std::string Name;
		std::string Value;
	};

	using ChildProcessOutputCallback = std::function<void(std::string_view)>;

	struct ChildProcessRequest final
	{
		std::filesystem::path ExecutablePath;
		std::vector<std::string> Arguments;
		std::filesystem::path WorkingDirectory;
		std::vector<EnvironmentOverride> Environment;
		std::filesystem::path LogPath;
		ChildProcessOutputCallback OutputCallback;
		std::stop_token Cancellation;
		std::optional<std::string> ReadinessValue;
		std::function<ChildReadinessDecision(std::uint32_t, std::string_view, std::string&)> ReadinessValidator;
	};

	struct ChildProcessResult final
	{
		std::uint32_t ProcessId = 0;
		bool Launched = false;
		bool Ready = false;
		bool Cancelled = false;
		int ExitCode = -1;
		ChildProcessStartFailure StartFailure = ChildProcessStartFailure::None;
		std::string CapturedOutput;
		std::string FailureReason;

		bool Succeeded() const noexcept { return Launched && !Cancelled && (Ready || ExitCode == 0) && FailureReason.empty(); }
	};

	class SPARKLE_CORE_API ChildProcess final
	{
	public:
		ChildProcess() = delete;

		static ChildProcessResult Run(const ChildProcessRequest& request);
	};
}
