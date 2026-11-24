#include "PCH.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"
#include "RHI/Public/Device/RhiDeviceLaunch.h"
#include "RHI/Public/Descriptors/RhiDescriptorService.h"

#include "Device/RenderDeviceServices.h"
#include "Device/RhiDeviceLaunch.h"
#include "Core/RhiBackendSelection.h"
#include "Core/RhiCapabilities.h"
#include "Diagnostics/RhiExternalCapture.h"
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
  #include "Diagnostics/GpuCrash/GpuCrashSession.h"
#endif
#include "Device/RenderDeviceBackendFactory.h"
#include "Device/RenderDeviceBackendServices.h"
#include "CVars/RHICVars.h"
#include "Presentation/RhiPresentationDefaults.h"

#include "Window/Window.h"

#include "Core/Public/Diagnostics/Logger.h"
#include "Core/Public/Diagnostics/Verify.h"
#include "Core/Public/Threading/ThreadOwnership.h"
#include <format>
#include <string>
#include <utility>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_renderDeviceServicesLogger, "RHI.Services");

struct RenderDeviceServices::State final
{
	Threading::OwnerThread Owner{"RenderDeviceServices"};
	std::unique_ptr<RenderDeviceBackendServices> Backend;
};

void RenderDeviceServices::FailCreation(std::string_view message) noexcept
{
	Diagnostics::Fatal(g_renderDeviceServicesLogger, __FILE__, __LINE__, message);
}

void RenderDeviceServices::FailUnsupportedBackend(ERhiBackendApi api) noexcept
{
	FailCreation(std::string("RHI backend '") + RhiBackendApiToString(api) + "' is not available in this RenderDeviceServices build.");
}

void RenderDeviceServices::ValidateBackBufferFormat(PixelFormat backBufferFormat) noexcept
{
	if (!RhiPresentationDefaults::IsSupportedBackBufferFormat(backBufferFormat))
	{
		FailCreation(std::string("Unsupported back buffer format for present swapchain: ") + PixelFormatName(backBufferFormat));
	}
}

RhiPresentationConfiguration RenderDeviceServices::ResolvePresentationConfiguration() noexcept
{
	const RhiPresentationConfiguration configuration{.BackBufferCount = CVarBackBufferCount.Get(), .MaximumFramesInFlight = CVarMaximumFramesInFlight.Get()};
	if (configuration.BackBufferCount < RhiPresentationDefaults::MinBackBufferCount || configuration.BackBufferCount > RhiPresentationDefaults::MaxBackBufferCount)
	{
		const std::string failureMessage = std::format(
		    "r.BackBufferCount={} is outside the DXGI flip-model and Sparkle presentation range [2, 3]. Use r.MaximumFramesInFlight=1 "
		    "for single-frame pacing.",
		    configuration.BackBufferCount);

		FailCreation(failureMessage);
	}
	if (configuration.MaximumFramesInFlight < RhiPresentationDefaults::MinFramesInFlight || configuration.MaximumFramesInFlight > RhiPresentationDefaults::MaxFramesInFlight)
	{
		const std::string failureMessage = std::format("r.MaximumFramesInFlight={} is outside the supported range [1, 3].", configuration.MaximumFramesInFlight);
		FailCreation(failureMessage);
	}
	if (configuration.MaximumFramesInFlight > configuration.BackBufferCount)
	{
		const std::string failureMessage = std::format("r.MaximumFramesInFlight={} exceeds r.BackBufferCount={}.", configuration.MaximumFramesInFlight, configuration.BackBufferCount);
		FailCreation(failureMessage);
	}
	return configuration;
}

RenderDeviceServices::RenderDeviceServices() noexcept :
    m_state(std::make_unique<State>())
{
}

RenderDeviceServices::~RenderDeviceServices() noexcept
{
	m_state->Owner.AssertAccess();
}

RenderDeviceBackendServices& RenderDeviceServices::GetBackendServices(std::source_location location) noexcept
{
	m_state->Owner.AssertAccess(location);
	return *m_state->Backend;
}

const RenderDeviceBackendServices& RenderDeviceServices::GetBackendServices(std::source_location location) const noexcept
{
	m_state->Owner.AssertAccess(location);
	return *m_state->Backend;
}

std::unique_ptr<RenderDeviceServices> RenderDeviceServices::Create(Window& window, const RhiDeviceLaunch& launch) noexcept
{
	const ERhiBackendApi backendApi = launch.BackendApi;
	const RhiInterposerHooks interposerHooks = launch.InterposerHooks;
	const PixelFormat backBufferFormat = CVarBackBufferFormat.Get();
	ValidateBackBufferFormat(backBufferFormat);
	const RhiPresentationConfiguration presentationConfiguration = ResolvePresentationConfiguration();

	auto services = std::unique_ptr<RenderDeviceServices>(new RenderDeviceServices());
	std::unique_ptr<RenderDeviceBackendServices> backendServices;
	switch (backendApi)
	{
		case ERhiBackendApi::D3D12:
#if SPARKLE_RHI_WITH_D3D12
			backendServices = CreateD3D12RenderDeviceServices(window, backBufferFormat, presentationConfiguration, interposerHooks);
			break;
#else
			FailUnsupportedBackend(backendApi);
			break;
#endif
		case ERhiBackendApi::Vulkan:
#if SPARKLE_RHI_WITH_VULKAN
			if (static_cast<bool>(interposerHooks))
			{
				FailCreation("Vulkan device creation received interposer hooks that only the D3D12 bridge implements.");
			}
			backendServices = CreateVulkanRenderDeviceServices(window, backBufferFormat, presentationConfiguration);
			break;
#else
			FailUnsupportedBackend(backendApi);
			break;
#endif
		case ERhiBackendApi::Unknown:
		default:
			FailUnsupportedBackend(backendApi);
	}

	services->m_state->Backend = std::move(backendServices);

#if SPARKLE_WITH_EXTERNAL_CAPTURE
	services->m_externalCapture = launch.ExternalCapture;
	if (launch.ExternalCapture)
	{
		services->GetBackendServices().BindExternalCapture(*launch.ExternalCapture);
	}
#endif
	const RhiAdapterIdentity& adapter = services->GetCapabilities().ExternalFeatureInterop.Adapter;

	SPDLOG_LOGGER_INFO(
	    g_renderDeviceServicesLogger,
	    "Device created: backend={} adapter='{}' vendor={:#06x} device={:#06x}",
	    RhiBackendApiToString(backendApi),
	    adapter.Name,
	    adapter.VendorId,
	    adapter.DeviceId);

	return services;
}

const RhiCapabilities& RenderDeviceServices::GetCapabilities() const noexcept
{
	const RenderDeviceBackendServices& backendServices = GetBackendServices();
	return backendServices.GetRenderHardwareInterface().GetCapabilities();
}

RenderHardwareInterface& RenderDeviceServices::GetRenderHardwareInterface() noexcept
{
	return GetBackendServices().GetRenderHardwareInterface();
}

const RenderHardwareInterface& RenderDeviceServices::GetRenderHardwareInterface() const noexcept
{
	const RenderDeviceBackendServices& backendServices = GetBackendServices();
	return backendServices.GetRenderHardwareInterface();
}

RhiImGuiRenderer& RenderDeviceServices::GetImGuiRenderer() noexcept
{
	return GetBackendServices().GetImGuiRenderer();
}

void RenderDeviceServices::SettleForShutdown() noexcept
{
#if SPARKLE_WITH_EXTERNAL_CAPTURE
	if (m_externalCapture)
	{
		m_externalCapture->Shutdown();
	}
#endif
	GetBackendServices().SettleForShutdown();
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
	// Settle diagnostics before cache joins can wait behind their writer on I/O.
	if (auto* session = GpuCrashSession::Current())
	{
		session->Shutdown();
	}
#endif
}

void RenderDeviceServices::ResizeSwapChain() noexcept
{
#if SPARKLE_WITH_EXTERNAL_CAPTURE
	if (m_externalCapture)
	{
		m_externalCapture->InvalidateTarget();
	}
#endif
	GetBackendServices().ResizeSwapChain();
}

void RenderDeviceServices::BeginFrame(std::uint64_t frameId) noexcept
{
	RenderDeviceBackendServices& backendServices = GetBackendServices();
#if SPARKLE_WITH_EXTERNAL_CAPTURE
	if (m_externalCapture)
	{
		m_externalCapture->BeginFrame(frameId);
	}
#endif
	backendServices.BeginFrame(frameId);
	RenderHardwareInterface& renderHardwareInterface = backendServices.GetRenderHardwareInterface();
	renderHardwareInterface.GetDescriptorService().BeginFrame(renderHardwareInterface.GetCurrentFrameIndex());
}

void RenderDeviceServices::PrepareCommandRecording() noexcept
{
	GetBackendServices().PrepareCommandRecording();
}

RenderCommandList& RenderDeviceServices::GetCurrentGraphicsCommandList() noexcept
{
	return GetBackendServices().GetCurrentGraphicsCommandList();
}

RenderCommandList& RenderDeviceServices::GetGraphicsCommandList(std::uint32_t frameIndex) noexcept
{
	return GetBackendServices().GetGraphicsCommandList(frameIndex);
}

RenderCommandList& RenderDeviceServices::BeginCurrentGraphicsCommandList() noexcept
{
	return GetBackendServices().BeginCurrentGraphicsCommandList();
}

RhiCommandRecordingLease RenderDeviceServices::AcquireCommandRecordingLease(ERhiQueueType queueType, RhiCommandRecordingOwner owner) noexcept
{
	return GetBackendServices().AcquireCommandRecordingLease(queueType, owner);
}

RhiCommandRecordingLease RenderDeviceServices::TakeCurrentGraphicsCommandRecordingLease() noexcept
{
	return GetBackendServices().TakeCurrentGraphicsCommandRecordingLease();
}

RhiSubmissionToken RenderDeviceServices::SubmitCommandRecordingLease(RhiCommandRecordingLease&& lease, std::span<const RhiSubmissionToken> waitTokens) noexcept
{
	return GetBackendServices().SubmitCommandRecordingLease(std::move(lease), waitTokens);
}

RhiSubmissionToken RenderDeviceServices::SubmitCommandRecordingBatch(std::span<RhiCommandRecordingLease> leases, std::span<const RhiSubmissionToken> waitTokens) noexcept
{
	return GetBackendServices().SubmitCommandRecordingBatch(leases, waitTokens);
}

RhiSubmissionToken RenderDeviceServices::SubmitCurrentGraphicsCommandList(std::span<const RhiSubmissionToken> waitTokens) noexcept
{
	return GetBackendServices().SubmitCurrentGraphicsCommandList(waitTokens);
}

void RenderDeviceServices::QueueWait(ERhiQueueType waitQueue, RhiSubmissionToken executionToken) noexcept
{
	GetBackendServices().QueueWait(waitQueue, executionToken);
}

void RenderDeviceServices::WaitForSubmission(RhiSubmissionToken token) noexcept
{
	GetBackendServices().WaitForSubmission(token);
}

bool RenderDeviceServices::IsSubmissionComplete(RhiSubmissionToken token) const noexcept
{
	return GetBackendServices().IsSubmissionComplete(token);
}

RhiSubmissionToken RenderDeviceServices::GetLastSubmittedToken(ERhiQueueType queueType) const noexcept
{
	return GetBackendServices().GetLastSubmittedToken(queueType);
}

void RenderDeviceServices::SubmitFrame(std::uint64_t frameId) noexcept
{
	GetBackendServices().SubmitFrame(frameId);
#if SPARKLE_WITH_EXTERNAL_CAPTURE
	if (m_externalCapture)
	{
		m_externalCapture->EndFrame();
	}
#endif
}

void RenderDeviceServices::AdvanceFrameInFlight() noexcept
{
	GetBackendServices().AdvanceFrameInFlight();
}
