#include "PCH.h"

#include "Editor/Capture/EditorViewportCaptureCoordinator.h"

#include "Core/Public/Paths/ProductUserStatePaths.h"
#include "Editor/Capture/ViewportCaptureWriter.h"
#include "Core/Public/Console/ConsoleOutput.h"
#include "Core/Public/Strings/StringUtils.h"
#include "Renderer.h"

#include <format>
#include <optional>
#include <utility>

EditorViewportCaptureCoordinator::EditorViewportCaptureCoordinator(TaskExecutor& executor, TaskScope& scope) noexcept :
    m_writeOperation(executor, scope)
{
}

std::optional<ConsoleOutputRecord> EditorViewportCaptureCoordinator::Request(Renderer& renderer, std::uint64_t frameId)
{
	if (!m_capture.IsSettled() || m_writeOperation.IsOccupied())
	{
		return ConsoleOutputRecord{ConsoleCommandSeverity::Error, "Viewport capture is already in progress."};
	}

	m_outputPath = BuildOutputPath(frameId);
	const ViewportCaptureAdmissionStatus admission = m_capture.Request(renderer, ViewportCaptureRequest{.Output = RenderOutputFlags::FinalColorLdr});
	if (admission != ViewportCaptureAdmissionStatus::Accepted)
	{
		m_outputPath.clear();
		const char* message = admission == ViewportCaptureAdmissionStatus::Full ? "Viewport capture rejected: render request capacity is full." : "Viewport capture rejected: render owner is closed.";

		return ConsoleOutputRecord{ConsoleCommandSeverity::Error, message};
	}
	return std::nullopt;
}

std::optional<ConsoleOutputRecord> EditorViewportCaptureCoordinator::Update(Renderer& renderer)
{
	m_capture.Update(renderer);
	if (m_capture.HasReadback())
	{
		ViewportCaptureReadback readback = m_capture.TakeReadback();
		if (!readback.Result)
		{
			return ConsoleOutputRecord{ConsoleCommandSeverity::Error, "Viewport capture failed: " + readback.Result.FailureReason};
		}

		std::string errorMessage;

		const std::string taskName = "Write viewport capture";

		auto operationTask = [readback = std::move(readback), outputPath = m_outputPath](ViewportCaptureResult& result, TaskExecutionContext& context) mutable
		{
			result = ViewportCaptureWriter::Write(std::move(readback), outputPath, context.GetCancellationToken());
			if (result)
			{
				return TaskResult::Success();
			}
			return context.IsCancellationRequested() ? TaskResult::Cancelled(result.FailureReason) : TaskResult::Failure(result.FailureReason);
		};

		const bool bOperationStarted = m_writeOperation.Start(taskName, "A viewport capture is already being written.", std::move(operationTask), errorMessage);

		if (!bOperationStarted)
		{
			return ConsoleOutputRecord{ConsoleCommandSeverity::Error, "Viewport capture could not start: " + errorMessage};
		}
	}

	ViewportCaptureResult result;
	if (const std::optional<TaskResult> taskResult = m_writeOperation.TryConsume(result))
	{
		if (!taskResult->Succeeded())
		{
			const char* status = taskResult->WasCancelled() ? "cancelled" : "failed";
			std::string message = std::format("Viewport capture {}: {}", status, taskResult->GetDiagnostic());
			return ConsoleOutputRecord{ConsoleCommandSeverity::Error, std::move(message)};
		}
		else
		{
			const std::string outputPath = Strings::ToNarrow(m_outputPath.wstring());
			return ConsoleOutputRecord{ConsoleCommandSeverity::Info, "Viewport captured: " + outputPath};
		}
	}
	return std::nullopt;
}

std::filesystem::path EditorViewportCaptureCoordinator::BuildOutputPath(std::uint64_t frameId) const
{
	const Filesystem::ProductUserStatePaths& userState = Filesystem::GetProductUserStatePaths();
	return userState.CapturesRoot / ("Viewport_" + std::to_string(frameId) + ".bmp");
}
