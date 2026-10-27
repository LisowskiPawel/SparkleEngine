#include "Vulkan/VulkanPCH.h"

#include "Device/RenderDeviceBackendFactory.h"
#if SPARKLE_WITH_EXTERNAL_CAPTURE
  #include "Diagnostics/ExternalCapture/ExternalCaptureAdapter.h"
#endif

#include "Vulkan/Commands/VulkanCommandRecordingContext.h"
#include "Vulkan/Commands/VulkanFrameSubmission.h"
#include "Vulkan/Descriptors/VulkanDescriptorService.h"
#include "Vulkan/Device/VulkanRhi.h"
#include "Vulkan/Memory/VulkanGpuMemoryAllocator.h"
#include "Vulkan/SwapChain/VulkanSwapChain.h"
#include "Vulkan/VulkanRenderHardwareInterface.h"

#include "Window/Window.h"

#include <span>

class VulkanRenderDeviceServices final : public RenderDeviceBackendServices
{
public:
	static std::unique_ptr<VulkanRenderDeviceServices> Create(Window& window, PixelFormat backBufferFormat, const RhiPresentationConfiguration& presentationConfiguration) noexcept;
	~VulkanRenderDeviceServices() noexcept override;

	VulkanRenderDeviceServices(const VulkanRenderDeviceServices&) = delete;
	VulkanRenderDeviceServices& operator=(const VulkanRenderDeviceServices&) = delete;
	VulkanRenderDeviceServices(VulkanRenderDeviceServices&&) = delete;
	VulkanRenderDeviceServices& operator=(VulkanRenderDeviceServices&&) = delete;

	RenderHardwareInterface& GetRenderHardwareInterface() noexcept override;
	const RenderHardwareInterface& GetRenderHardwareInterface() const noexcept override;
	RhiImGuiRenderer& GetImGuiRenderer() noexcept override;
	void SettleForShutdown() noexcept override;
#if SPARKLE_WITH_EXTERNAL_CAPTURE
	void BindExternalCapture(RhiExternalCapture& capture) noexcept override;
#endif
	void ResizeSwapChain() noexcept override;
	void BeginFrame(std::uint64_t frameId) noexcept override;
	void PrepareCommandRecording() noexcept override;
	RenderCommandList& GetCurrentGraphicsCommandList() noexcept override;
	RenderCommandList& GetGraphicsCommandList(std::uint32_t frameIndex) noexcept override;
	RenderCommandList& BeginCurrentGraphicsCommandList() noexcept override;
	RhiCommandRecordingLease AcquireCommandRecordingLease(ERhiQueueType queueType, RhiCommandRecordingOwner owner) noexcept override;
	RhiCommandRecordingLease TakeCurrentGraphicsCommandRecordingLease() noexcept override;
	RhiSubmissionToken SubmitCommandRecordingLease(RhiCommandRecordingLease&& lease, std::span<const RhiSubmissionToken> waitTokens) noexcept override;
	RhiSubmissionToken SubmitCommandRecordingBatch(std::span<RhiCommandRecordingLease> leases, std::span<const RhiSubmissionToken> waitTokens) noexcept override;
	RhiSubmissionToken SubmitCurrentGraphicsCommandList(std::span<const RhiSubmissionToken> waitTokens) noexcept override;
	void QueueWait(ERhiQueueType waitQueue, RhiSubmissionToken executionToken) noexcept override;
	void WaitForSubmission(RhiSubmissionToken token) noexcept override;
	bool IsSubmissionComplete(RhiSubmissionToken token) const noexcept override;
	RhiSubmissionToken GetLastSubmittedToken(ERhiQueueType queueType) const noexcept override;
	void SubmitFrame(std::uint64_t frameId) noexcept override;
	void AdvanceFrameInFlight() noexcept override;

private:
	VulkanRenderDeviceServices() noexcept = default;

	void Initialize(Window& window, PixelFormat backBufferFormat, const RhiPresentationConfiguration& presentationConfiguration);
	void InitializeDevice();
	void InitializePresentation(Window& window, PixelFormat backBufferFormat, const RhiPresentationConfiguration& presentationConfiguration);
	void InitializeHardwareInterface();
	void BeginFrameRecording();
	void AcquireFrameBackBuffer();

	std::unique_ptr<VulkanRhi> m_rhi;
	std::unique_ptr<VulkanGpuMemoryAllocator> m_memoryAllocator;
	std::unique_ptr<VulkanSwapChain> m_swapChain;
	std::unique_ptr<VulkanCommandRecordingContext> m_commandRecordingContext;
	std::unique_ptr<VulkanRenderHardwareInterface> m_renderHardwareInterface;
	std::unique_ptr<VulkanFrameSubmission> m_frameSubmission;
	std::uint32_t m_currentFrameIndex = 0;
};

std::unique_ptr<RenderDeviceBackendServices> CreateVulkanRenderDeviceServices(Window& window, PixelFormat backBufferFormat, const RhiPresentationConfiguration& presentationConfiguration) noexcept
{
	return VulkanRenderDeviceServices::Create(window, backBufferFormat, presentationConfiguration);
}

std::unique_ptr<VulkanRenderDeviceServices> VulkanRenderDeviceServices::Create(Window& window, PixelFormat backBufferFormat, const RhiPresentationConfiguration& presentationConfiguration) noexcept
{
	auto services = std::unique_ptr<VulkanRenderDeviceServices>(new VulkanRenderDeviceServices());
	services->Initialize(window, backBufferFormat, presentationConfiguration);
	return services;
}

void VulkanRenderDeviceServices::Initialize(Window& window, PixelFormat backBufferFormat, const RhiPresentationConfiguration& presentationConfiguration)
{
	InitializeDevice();
	InitializePresentation(window, backBufferFormat, presentationConfiguration);
	InitializeHardwareInterface();
}

void VulkanRenderDeviceServices::InitializeDevice()
{
	m_rhi = std::make_unique<VulkanRhi>();
	m_memoryAllocator = std::make_unique<VulkanGpuMemoryAllocator>(*m_rhi);
}

void VulkanRenderDeviceServices::InitializePresentation(Window& window, PixelFormat backBufferFormat, const RhiPresentationConfiguration& presentationConfiguration)
{
	m_swapChain = std::make_unique<VulkanSwapChain>(*m_rhi, window, backBufferFormat, presentationConfiguration);
}

void VulkanRenderDeviceServices::InitializeHardwareInterface()
{
	m_renderHardwareInterface = std::make_unique<VulkanRenderHardwareInterface>(*m_rhi, *m_swapChain, *m_memoryAllocator);

	m_commandRecordingContext = std::make_unique<VulkanCommandRecordingContext>(*m_rhi, *m_memoryAllocator, *m_renderHardwareInterface->m_descriptorService, m_swapChain->GetMaximumFramesInFlight());
	m_renderHardwareInterface->SetCommandRecordingContext(*m_commandRecordingContext);
	m_frameSubmission = std::make_unique<VulkanFrameSubmission>(*m_rhi, *m_swapChain, *m_commandRecordingContext, m_currentFrameIndex);
}

VulkanRenderDeviceServices::~VulkanRenderDeviceServices() noexcept
{
	m_frameSubmission.reset();
	m_commandRecordingContext.reset();
	m_renderHardwareInterface.reset();
	m_swapChain.reset();
	m_memoryAllocator.reset();
	m_rhi.reset();
}

RenderHardwareInterface& VulkanRenderDeviceServices::GetRenderHardwareInterface() noexcept
{
	return *m_renderHardwareInterface;
}

const RenderHardwareInterface& VulkanRenderDeviceServices::GetRenderHardwareInterface() const noexcept
{
	return *m_renderHardwareInterface;
}

RhiImGuiRenderer& VulkanRenderDeviceServices::GetImGuiRenderer() noexcept
{
	return m_renderHardwareInterface->GetImGuiRenderer();
}

void VulkanRenderDeviceServices::SettleForShutdown() noexcept
{
	m_frameSubmission->ResetPresentation();
	m_renderHardwareInterface->WaitForIdle();
}

void VulkanRenderDeviceServices::ResizeSwapChain() noexcept
{
	m_frameSubmission->DrainForSwapChainRecreation();
	m_swapChain->Resize();
	m_renderHardwareInterface->RebuildSwapChainBackBufferViews();
}

void VulkanRenderDeviceServices::BeginFrame(std::uint64_t frameId) noexcept
{
	(void) frameId;
	m_renderHardwareInterface->SetCurrentFrameIndex(m_currentFrameIndex);
	BeginFrameRecording();
	AcquireFrameBackBuffer();
}

void VulkanRenderDeviceServices::PrepareCommandRecording() noexcept
{
	m_renderHardwareInterface->m_descriptorService->PublishRecordingReadView();
	m_memoryAllocator->PublishRecordingReadView();
}

void VulkanRenderDeviceServices::BeginFrameRecording()
{
	m_frameSubmission->BeginFrame();

	m_commandRecordingContext->BeginFrame(m_currentFrameIndex);
	m_renderHardwareInterface->ResetTransientFrameResources();
	(void) BeginCurrentGraphicsCommandList();
}

void VulkanRenderDeviceServices::AcquireFrameBackBuffer()
{
	bool acquired = m_frameSubmission->AcquireBackBuffer();
	if (!acquired && m_swapChain->ConsumeResizeRequest())
	{
		ResizeSwapChain();
		acquired = m_frameSubmission->AcquireBackBuffer();
	}
	if (!acquired)
	{
		m_commandRecordingContext->CancelFrame(m_currentFrameIndex);
		m_renderHardwareInterface->RebuildSwapChainBackBufferViews();
		return;
	}

	m_renderHardwareInterface->PrepareAcquiredBackBuffer();
}

RenderCommandList& VulkanRenderDeviceServices::GetCurrentGraphicsCommandList() noexcept
{
	return m_renderHardwareInterface->GetGraphicsCommandList(m_currentFrameIndex);
}

RenderCommandList& VulkanRenderDeviceServices::GetGraphicsCommandList(std::uint32_t frameIndex) noexcept
{
	return m_renderHardwareInterface->GetGraphicsCommandList(frameIndex);
}

RenderCommandList& VulkanRenderDeviceServices::BeginCurrentGraphicsCommandList() noexcept
{
	return m_commandRecordingContext->BeginCurrentGraphicsCommandList(m_currentFrameIndex);
}

RhiCommandRecordingLease VulkanRenderDeviceServices::AcquireCommandRecordingLease(ERhiQueueType queueType, RhiCommandRecordingOwner owner) noexcept
{
	return m_commandRecordingContext->Acquire(queueType, m_currentFrameIndex, owner);
}

RhiCommandRecordingLease VulkanRenderDeviceServices::TakeCurrentGraphicsCommandRecordingLease() noexcept
{
	return m_commandRecordingContext->TakeCurrentGraphicsCommandRecordingLease(m_currentFrameIndex);
}

RhiSubmissionToken VulkanRenderDeviceServices::SubmitCommandRecordingLease(RhiCommandRecordingLease&& lease, std::span<const RhiSubmissionToken> waitTokens) noexcept
{
	return m_frameSubmission->Submit(std::move(lease), waitTokens);
}

RhiSubmissionToken VulkanRenderDeviceServices::SubmitCommandRecordingBatch(std::span<RhiCommandRecordingLease> leases, std::span<const RhiSubmissionToken> waitTokens) noexcept
{
	return m_frameSubmission->SubmitBatch(leases, waitTokens);
}

RhiSubmissionToken VulkanRenderDeviceServices::SubmitCurrentGraphicsCommandList(std::span<const RhiSubmissionToken> waitTokens) noexcept
{
	return m_frameSubmission->SubmitCurrentGraphicsCommandList(waitTokens);
}

void VulkanRenderDeviceServices::QueueWait(ERhiQueueType waitQueue, RhiSubmissionToken executionToken) noexcept
{
	m_frameSubmission->QueueWait(waitQueue, executionToken);
}

void VulkanRenderDeviceServices::WaitForSubmission(RhiSubmissionToken token) noexcept
{
	m_frameSubmission->WaitForSubmission(token);
}

bool VulkanRenderDeviceServices::IsSubmissionComplete(RhiSubmissionToken token) const noexcept
{
	return m_frameSubmission->IsSubmissionComplete(token);
}

RhiSubmissionToken VulkanRenderDeviceServices::GetLastSubmittedToken(ERhiQueueType queueType) const noexcept
{
	return m_frameSubmission->GetLastSubmittedToken(queueType);
}

void VulkanRenderDeviceServices::SubmitFrame(std::uint64_t frameId) noexcept
{
	(void) frameId;
	if (!m_frameSubmission->HasAcquiredBackBuffer())
	{
		return;
	}

	m_renderHardwareInterface->PrepareCurrentBackBufferForPresentation();
	if (m_frameSubmission->SubmitAndPresent())
	{
		ResizeSwapChain();
	}
}

void VulkanRenderDeviceServices::AdvanceFrameInFlight() noexcept
{
	m_currentFrameIndex = (m_currentFrameIndex + 1u) % m_swapChain->GetMaximumFramesInFlight();
	m_renderHardwareInterface->SetCurrentFrameIndex(m_currentFrameIndex);
}

#if SPARKLE_WITH_EXTERNAL_CAPTURE
void VulkanRenderDeviceServices::BindExternalCapture(RhiExternalCapture& capture) noexcept
{
	// Vulkan uses the dispatch-table root inside VkInstance, as specified by
	// RenderDoc's RENDERDOC_DEVICEPOINTER_FROM_VKINSTANCE, never VkDevice.
	RhiExternalCaptureBinding::Bind(capture, *reinterpret_cast<void**>(m_rhi->GetInstance()), m_swapChain->GetHostWindow().GetHWND());
}
#endif
