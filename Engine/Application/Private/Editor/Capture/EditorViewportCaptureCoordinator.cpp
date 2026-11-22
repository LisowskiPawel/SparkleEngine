#include "PCH.h"

#include "Editor/Capture/EditorViewportCaptureCoordinator.h"

#include "Core/Public/Paths/ProductUserStatePaths.h"
#include "Editor/Capture/ViewportCaptureWriter.h"
#include "Core/Public/Console/ConsoleOutput.h"
#include "Core/Public/Strings/StringUtils.h"
#include "Renderer.h"

#include <TaskScheduler_c.h>
#include "Core/Public/Diagnostics/Verify.h"
#include <new>
#include <exception>
#include <format>
#include <optional>
#include <utility>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_editorCaptureLogger, "Application.Capture");

struct EditorViewportCaptureCoordinator::Write final
{
	ViewportCaptureReadback Readback;
	std::filesystem::path OutputPath;
	std::stop_token Stop;
	ViewportCaptureResult Result;
	enkiPinnedTask* Task = nullptr;
};

EditorViewportCaptureCoordinator::EditorViewportCaptureCoordinator(enkiTaskScheduler* io, std::stop_token stop) noexcept :
    m_io(io),
    m_stop(stop)
{
}

EditorViewportCaptureCoordinator::~EditorViewportCaptureCoordinator()
{
	if (m_write)
	{
		Diagnostics::Fatal(g_editorCaptureLogger, __FILE__, __LINE__, "Viewport capture work must be consumed or explicitly shut down before owner destruction.");
	}
}

void EditorViewportCaptureCoordinator::Shutdown() noexcept
{
	if (m_write)
	{
		if (m_write->Task)
		{
			enkiWaitForPinnedTask(m_io, m_write->Task);
			enkiDeletePinnedTask(m_io, m_write->Task);
		}
		m_write.reset();
	}
	m_capture.Discard();
}

void EditorViewportCaptureCoordinator::WriteCapture(void* argument) noexcept
{
	auto& write = *static_cast<Write*>(argument);
	try
	{
		write.Result = ViewportCaptureWriter::Write(std::move(write.Readback), write.OutputPath, write.Stop);
	}
	catch (const std::exception& error)
	{
		write.Result.Status = ViewportCaptureStatus::Failed;
		write.Result.FailureReason = error.what();
	}
	catch (...)
	{
		write.Result.Status = ViewportCaptureStatus::Failed;
		write.Result.FailureReason = "Unknown viewport capture write failure.";
	}
}

std::optional<ConsoleOutputRecord> EditorViewportCaptureCoordinator::Request(Renderer& renderer, std::uint64_t frameId)
{
	if (m_stop.stop_requested())
	{
		return ConsoleOutputRecord{ConsoleCommandSeverity::Error, "Viewport capture owner is closed."};
	}
	if (!m_capture.IsSettled() || m_write != nullptr)
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

		try
		{
			auto write = std::make_unique<Write>(std::move(readback), m_outputPath, m_stop);
			PrepareNativeWork(*write);
			m_write = std::move(write);
			if (m_io)
			{
				enkiAddPinnedTask(m_io, m_write->Task);
			}
			else
			{
				WriteCapture(m_write.get());
			}
		}
		catch (const std::exception& error)
		{
			return ConsoleOutputRecord{ConsoleCommandSeverity::Error, "Viewport capture could not start: " + std::string(error.what())};
		}
	}
	if (m_write)
	{
		if (m_write->Task && !enkiIsPinnedTaskComplete(m_io, m_write->Task))
		{
			return std::nullopt;
		}
		auto result = std::move(m_write->Result);
		if (m_write->Task)
		{
			enkiDeletePinnedTask(m_io, m_write->Task);
		}
		m_write.reset();
		if (!result)
		{
			return ConsoleOutputRecord{ConsoleCommandSeverity::Error, "Viewport capture failed: " + result.FailureReason};
		}
		const std::string outputPath = Strings::ToNarrow(m_outputPath.wstring());
		return ConsoleOutputRecord{ConsoleCommandSeverity::Info, "Viewport captured: " + outputPath};
	}

	return std::nullopt;
}

std::filesystem::path EditorViewportCaptureCoordinator::BuildOutputPath(std::uint64_t frameId) const
{
	const Filesystem::ProductUserStatePaths& userState = Filesystem::GetProductUserStatePaths();
	return userState.CapturesRoot / ("Viewport_" + std::to_string(frameId) + ".bmp");
}

void EditorViewportCaptureCoordinator::PrepareNativeWork(Write& write)
{
	if (!m_io)
	{
		return;
	}

	const auto config = enkiGetTaskSchedulerConfig(m_io);
	write.Task = enkiCreatePinnedTask(m_io, WriteCapture, config.numExternalTaskThreads + 1);
	if (!write.Task)
	{
		throw std::bad_alloc();
	}
	enkiSetArgsPinnedTask(write.Task, &write);
}
