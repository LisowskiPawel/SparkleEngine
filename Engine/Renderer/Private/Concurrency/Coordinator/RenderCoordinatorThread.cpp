#include "PCH.h"
#include "Concurrency/Coordinator/RenderCoordinator.h"

#include "Concurrency/Coordinator/RendererExecutionContext.h"

#include <exception>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_renderCoordinatorLogger, "Renderer.Coordinator");

void RenderCoordinator::Initialize()
{
	if (m_config.IsThreaded())
	{
		InitializeThreaded();
	}
	else
	{
		InitializeSerial();
	}
}

void RenderCoordinator::InitializeSerial()
{
	m_context = std::make_unique<RendererExecutionContext>(*m_window, m_deviceLaunch, m_config);
	SubmitResize();
	PublishReadState();
}

void RenderCoordinator::InitializeThreaded()
{
	m_frameQueue = std::make_unique<RenderFrameQueue>(m_config.GetFrameQueueCapacity());
	m_threadCommandQueue = std::make_unique<RenderThreadCommandQueue>(RenderThreadCommandCapacity);
	StartRenderThread();
	if (!WaitForRenderThreadStart())
	{
		HandleRenderThreadStartFailure();
		return;
	}

	SubmitResize();
}

void RenderCoordinator::StartRenderThread()
{
	m_renderThread = std::thread([this] { RenderThreadMain(); });
}

bool RenderCoordinator::WaitForRenderThreadStart()
{
	std::unique_lock lock(m_startMutex);
	m_startedCondition.wait(lock, [this] { return m_bStarted; });

	return m_bStartSucceeded;
}

void RenderCoordinator::HandleRenderThreadStartFailure()
{
	if (m_renderThread.joinable())
	{
		m_renderThread.join();
	}
	if (m_startFailure)
	{
		std::rethrow_exception(m_startFailure);
	}

	Diagnostics::Fatal(g_renderCoordinatorLogger, __FILE__, __LINE__, "RenderThread failed to create its renderer execution context.");
}

void RenderCoordinator::RenderThreadMain()
{
	Threading::SetCurrentThreadRole("Sparkle.RenderThread");
	std::exception_ptr failure;
	try
	{
		InitializeRenderThreadContext();
		ProcessRenderThreadCommands();
	}
	catch (const std::exception& exception)
	{
		failure = std::current_exception();
		SPDLOG_ERROR("RenderThread terminated after an exception: {}", exception.what());
	}
	catch (...)
	{
		failure = std::current_exception();
		SPDLOG_ERROR("RenderThread terminated after an unknown exception.");
	}

	FinishRenderThread(std::move(failure));
}

void RenderCoordinator::InitializeRenderThreadContext()
{
	m_context = std::make_unique<RendererExecutionContext>(*m_window, m_deviceLaunch, m_config);
	{
		std::lock_guard lock(m_startMutex);
		m_bStartSucceeded = true;
		m_bStarted = true;
	}
	m_startedCondition.notify_one();
	PublishReadState();
}

void RenderCoordinator::ProcessRenderThreadCommands()
{
	while (std::optional<RenderThreadCommand> command = m_threadCommandQueue->WaitPop())
	{
		ProcessThreadedCommand(std::move(*command));
	}
}

void RenderCoordinator::FinishRenderThread(std::exception_ptr failure) noexcept
{
	{
		std::lock_guard lock(m_startMutex);
		if (!m_bStartSucceeded)
		{
			m_startFailure = std::move(failure);
		}
		m_bStarted = true;
	}
	m_startedCondition.notify_one();
	SettleAbandonedWork();
	m_context.reset();
}

void RenderCoordinator::ProcessThreadedCommand(RenderThreadCommand command)
{
	if (command.SequenceNumber <= m_lastConsumedThreadCommandSequence)
	{
		Diagnostics::Fatal(g_renderCoordinatorLogger, __FILE__, __LINE__, "Render-thread command sequence was consumed more than once or out of order.");
	}
	m_lastConsumedThreadCommandSequence = command.SequenceNumber;
	if (const auto* frame = std::get_if<RenderFrameReadyCommand>(&command.Payload))
	{
		ExecuteThreadedFrame(frame->Ticket);
	}
	else
	{
		m_context->ExecuteControl(std::move(std::get<RendererExecutionControl>(command.Payload)));
		PublishReadState();
	}
}

void RenderCoordinator::ExecuteThreadedFrame(RenderFrameQueueTicket ticket)
{
	RenderExecutionRequest request;
	if (!m_frameQueue->Consume(ticket, request))
	{
		Diagnostics::Fatal(g_renderCoordinatorLogger, __FILE__, __LINE__, "Render frame queue rejected a queued frame ticket.");
	}
	m_context->ExecuteFrame(std::move(request));
	PublishReadState();
	if (!m_frameQueue->Retire(ticket))
	{
		Diagnostics::Fatal(g_renderCoordinatorLogger, __FILE__, __LINE__, "Render frame queue rejected retirement for the frame being rendered.");
	}
}

void RenderCoordinator::SettleAbandonedWork() noexcept
{
	CancelPendingControls();
	m_frameQueue->SettleAll();
	FailOutstandingViewportCaptures();
}
