#include "PCH.h"

#include "Editor/Capture/EditorViewportCaptureCoordinator.h"

#include "Core/Public/Paths/ProductUserStatePaths.h"
#include "Editor/Capture/ViewportCaptureWriter.h"
#include "Core/Public/Console/ConsoleOutput.h"
#include "Core/Public/Strings/StringUtils.h"
#include "Editor/Public/Console/EditorConsoleSystem.h"
#include "Renderer.h"

#include <format>
#include <optional>
#include <utility>

EditorViewportCaptureCoordinator::EditorViewportCaptureCoordinator(EditorOperationRuntime& operations) noexcept :
    m_writeOperation(operations)
{
}

void EditorViewportCaptureCoordinator::Request(EditorConsoleSystem* console, Renderer& renderer, std::uint64_t frameId)
{
	if (!m_capture.IsSettled() || m_writeOperation.IsOccupied())
	{
		PublishStatus(console, "Viewport capture is already in progress.", true);
		return;
	}

	m_outputPath = BuildOutputPath(frameId);
	const ViewportCaptureAdmissionStatus admission = m_capture.Request(renderer, ViewportCaptureRequest{.Output = RenderOutputFlags::FinalColorLdr});
	if (admission != ViewportCaptureAdmissionStatus::Accepted)
	{
		PublishStatus(
		    console,
		    admission == ViewportCaptureAdmissionStatus::Full ? "Viewport capture rejected: render request capacity is full." : "Viewport capture rejected: render owner is closed.",
		    true);

		m_outputPath.clear();
	}
}

void EditorViewportCaptureCoordinator::Update(EditorConsoleSystem* console, Renderer& renderer)
{
	m_capture.Update(renderer);
	if (m_capture.HasReadback())
	{
		ViewportCaptureReadback readback = m_capture.TakeReadback();
		if (!readback.Result)
		{
			PublishStatus(console, "Viewport capture failed: " + readback.Result.FailureReason, true);
			return;
		}

		std::string errorMessage;

		if (!m_writeOperation.Start(
		        TaskName("Write viewport capture"),
		        "A viewport capture is already being written.",
		        [readback = std::move(readback), outputPath = m_outputPath](ViewportCaptureResult& result, TaskExecutionContext& context) mutable
		        {
			        result = ViewportCaptureWriter::Write(std::move(readback), outputPath, context.GetCancellationToken());
			        return result ? TaskResult::Success() : (context.IsCancellationRequested() ? TaskResult::Cancelled(result.FailureReason) : TaskResult::Failure(result.FailureReason));
		        },
		        errorMessage))
		{
			PublishStatus(console, "Viewport capture could not start: " + errorMessage, true);
		}
	}

	ViewportCaptureResult result;
	if (const std::optional<TaskResult> taskResult = m_writeOperation.TryConsume(result))
	{
		if (!taskResult->Succeeded())
		{
			PublishStatus(console, std::format("Viewport capture {}: {}", taskResult->WasCancelled() ? "cancelled" : "failed", taskResult->GetMessage()), true);
		}
		else
		{
			PublishStatus(console, "Viewport captured: " + Strings::ToNarrow(m_outputPath.wstring()), false);
		}
	}
}

void EditorViewportCaptureCoordinator::PublishStatus(EditorConsoleSystem* console, std::string status, bool failed)
{
	if (console != nullptr)
	{
		console->AppendOutput(ConsoleOutputRecord{.Severity = failed ? ConsoleCommandSeverity::Error : ConsoleCommandSeverity::Info, .Text = std::move(status)});
		console->OpenConsole();
	}
}

std::filesystem::path EditorViewportCaptureCoordinator::BuildOutputPath(std::uint64_t frameId) const
{
	const Filesystem::ProductUserStatePaths& userState = Filesystem::GetProductUserStatePaths();
	return userState.CapturesRoot / ("Viewport_" + std::to_string(frameId) + ".bmp");
}
