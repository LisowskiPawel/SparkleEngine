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

EditorViewportCaptureCoordinator::EditorViewportCaptureCoordinator(EditorOperationRuntime& operations) noexcept :
    m_writeOperation(operations)
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

		return ConsoleOutputRecord{
		    ConsoleCommandSeverity::Error,
		    admission == ViewportCaptureAdmissionStatus::Full ? "Viewport capture rejected: render request capacity is full." : "Viewport capture rejected: render owner is closed."};
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

		const TaskName taskName = TaskName("Write viewport capture");

		auto operationTask = [readback = std::move(readback), outputPath = m_outputPath](ViewportCaptureResult& result, TaskExecutionContext& context) mutable
		{
			result = ViewportCaptureWriter::Write(std::move(readback), outputPath, context.GetCancellationToken());
			return result ? TaskResult::Success() : (context.IsCancellationRequested() ? TaskResult::Cancelled(result.FailureReason) : TaskResult::Failure(result.FailureReason));
		};

		const bool operationStarted = m_writeOperation.Start(taskName, "A viewport capture is already being written.", std::move(operationTask), errorMessage);

		if (!operationStarted)
		{
			return ConsoleOutputRecord{ConsoleCommandSeverity::Error, "Viewport capture could not start: " + errorMessage};
		}
	}

	ViewportCaptureResult result;
	if (const std::optional<TaskResult> taskResult = m_writeOperation.TryConsume(result))
	{
		if (!taskResult->Succeeded())
		{
			return ConsoleOutputRecord{ConsoleCommandSeverity::Error, std::format("Viewport capture {}: {}", taskResult->WasCancelled() ? "cancelled" : "failed", taskResult->GetMessage())};
		}
		else
		{
			return ConsoleOutputRecord{ConsoleCommandSeverity::Info, "Viewport captured: " + Strings::ToNarrow(m_outputPath.wstring())};
		}
	}
	return std::nullopt;
}

std::filesystem::path EditorViewportCaptureCoordinator::BuildOutputPath(std::uint64_t frameId) const
{
	const Filesystem::ProductUserStatePaths& userState = Filesystem::GetProductUserStatePaths();
	return userState.CapturesRoot / ("Viewport_" + std::to_string(frameId) + ".bmp");
}
