#pragma once

#include "Core/Public/Process/ChildProcess.h"
#include "SparkleLauncher/GpuCrashIncident.h"

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>

namespace SparkleLauncher
{
	enum class OperationStatus : std::uint8_t
	{
		Pending,
		Running,
		Succeeded,
		Failed,
		Skipped,
		Canceled
	};

	enum class OperationDestructiveScope : std::uint8_t
	{
		None,
		CookedOutputs,
		BuildTree,
		ArtifactOutputs,
		WorkspaceState,
		DependencyCache,
		Logs,
		PristineGeneratedWorkspace
	};

	enum class OperationProblemKind : std::uint8_t
	{
		None,
		Prerequisite,
		Planning,
		ProcessStart,
		ToolExecution,
		OutputValidation,
		Filesystem,
		Internal,
		Cancellation
	};

	struct OperationFailure final
	{
		OperationProblemKind Kind = OperationProblemKind::None;
		std::string Summary;
		std::string ExpectedAction;
	};

	struct OperationRecord
	{
		std::string Id;
		std::string DisplayName;
		OperationStatus Status = OperationStatus::Pending;
		std::filesystem::path LogPath;
		std::chrono::system_clock::time_point StartTime;
		std::chrono::system_clock::time_point EndTime;
		std::optional<int> ExitCode;
		OperationDestructiveScope DestructiveScope = OperationDestructiveScope::None;
		bool bRequiresConfirmation = false;
		std::optional<OperationFailure> Failure;
		std::optional<GpuCrashIncident> GpuCrash;
		std::filesystem::path ShaderExportPath;
	};

	OperationRecord MakeOperationRecord(std::string id, std::string displayName);
	void MarkOperationStarted(OperationRecord& operation, std::filesystem::path logPath = {});
	void MarkOperationFinished(OperationRecord& operation, OperationStatus status, std::optional<int> exitCode = std::nullopt);
	void SetOperationFailure(OperationRecord& operation, OperationProblemKind kind, std::string summary, std::string expectedAction);
	void SetProcessOperationFailure(OperationRecord& operation, Process::ChildProcessStartFailure startFailure, std::string summary, std::string retryAction);
	std::string ToString(OperationStatus status);
	std::string ToString(OperationDestructiveScope scope);
	std::string ToString(OperationProblemKind kind);
}
