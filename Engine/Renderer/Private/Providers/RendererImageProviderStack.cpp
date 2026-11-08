#include "../PCH.h"
#include "Providers/RendererImageProviderStack.h"

#include "Providers/ImageProviderFrameInput.h"
#include "Frame/RenderFrameIdentity.h"
#include "View/RenderView.h"
#include "RayReconstruction/RayReconstructionProvider.h"
#include "RayReconstruction/RayReconstructionProviderFactory.h"
#include "RayReconstruction/RayReconstructionSettings.h"
#include "Core/Public/Diagnostics/Verify.h"
#include "RHI/Public/Device/RenderDeviceServices.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"
#include "Upscaling/UpscalerProvider.h"
#include "Upscaling/UpscalerProviderFactory.h"
#include "Upscaling/UpscalerSettings.h"

#include <algorithm>
#include <array>
#include <limits>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_rendererImageProviderStackLogger, "Renderer.ImageProviders");

RendererImageProviderStack::RendererImageProviderStack(RenderHardwareInterface& renderHardwareInterface, RenderDeviceServices& deviceServices) :
    m_renderHardwareInterface(renderHardwareInterface),
    m_deviceServices(deviceServices)
{
	Initialize();
}

RendererImageProviderStack::~RendererImageProviderStack() noexcept
{
	Shutdown();
}

void RendererImageProviderStack::Initialize()
{
	m_upscaler = CreateConfiguredUpscalerProvider();
	m_rayReconstruction = CreateConfiguredRayReconstructionProvider();
	InitializeUpscaler();
	InitializeRayReconstruction();
	m_bResetHistoryPending = true;
}

void RendererImageProviderStack::InitializeUpscaler()
{
	const RhiCapabilities& capabilities = m_renderHardwareInterface.GetCapabilities();
	RhiInteropService& interop = m_renderHardwareInterface.GetInteropService();

	if (m_upscaler != nullptr)
	{
		const RhiNativeInteropRequest request{.Consumer = ERhiNativeInteropConsumer::ExternalProvider, .Reason = "Renderer upscaler provider initialization"};
		const auto nativeInterop = interop.GetDeviceQueueInterop(request);
		const bool bInitialized = m_upscaler->Initialize(capabilities, nativeInterop);
		if (!bInitialized)
		{
			Diagnostics::Fatal(
			    g_rendererImageProviderStackLogger,
			    __FILE__,
			    __LINE__,
			    "The selected renderer upscaler failed initialization on the selected backend and adapter. Selection is unchanged; choose a "
			    "supported provider or repair its prerequisites.");
		}
	}
}

void RendererImageProviderStack::InitializeRayReconstruction()
{
	const RhiCapabilities& capabilities = m_renderHardwareInterface.GetCapabilities();
	RhiInteropService& interop = m_renderHardwareInterface.GetInteropService();

	if (m_rayReconstruction != nullptr)
	{
		const RhiNativeInteropRequest request{.Consumer = ERhiNativeInteropConsumer::ExternalProvider, .Reason = "Renderer ray-reconstruction provider initialization"};
		const auto nativeInterop = interop.GetDeviceQueueInterop(request);
		const bool bInitialized = m_rayReconstruction->Initialize(capabilities, nativeInterop);
		if (!bInitialized)
		{
			Diagnostics::Fatal(
			    g_rendererImageProviderStackLogger,
			    __FILE__,
			    __LINE__,
			    "The selected ray-reconstruction provider failed initialization on the selected backend and adapter. Selection is unchanged; "
			    "choose a supported provider or repair its prerequisites.");
		}
	}
}

void RendererImageProviderStack::Shutdown() noexcept
{
	ShutdownProviders(m_upscaler, m_rayReconstruction);
	for (RetiredGeneration& generation : m_retiredGenerations)
	{
		ShutdownProviders(generation.Upscaler, generation.RayReconstruction);
	}
	m_retiredGenerations.clear();
}

void RendererImageProviderStack::ShutdownProviders(std::unique_ptr<IUpscalerProvider>& upscaler, std::unique_ptr<IRayReconstructionProvider>& rayReconstruction) noexcept
{
	if (upscaler != nullptr)
	{
		upscaler->Shutdown();
		upscaler.reset();
	}
	if (rayReconstruction != nullptr)
	{
		rayReconstruction->Shutdown();
		rayReconstruction.reset();
	}
}

void RendererImageProviderStack::Refresh() noexcept
{
	if (m_generation == (std::numeric_limits<std::uint64_t>::max)())
	{
		Diagnostics::Fatal(g_rendererImageProviderStackLogger, __FILE__, __LINE__, "Renderer image-provider generation exhausted.");
	}

	RhiSubmissionState lastUse;
	for (std::size_t queueIndex = 0; queueIndex < RhiQueueTypeCount; ++queueIndex)
	{
		lastUse.MarkUsed(m_deviceServices.GetLastSubmittedToken(static_cast<ERhiQueueType>(queueIndex)));
	}

	m_retiredGenerations.push_back(RetiredGeneration{.LastUse = lastUse, .Upscaler = std::move(m_upscaler), .RayReconstruction = std::move(m_rayReconstruction)});

	Initialize();
	++m_generation;
}

void RendererImageProviderStack::PollRetiredGenerations() noexcept
{
	m_retiredGenerations.erase(
	    std::remove_if(
	        m_retiredGenerations.begin(),
	        m_retiredGenerations.end(),
	        [this](RetiredGeneration& generation) noexcept
	        {
		        std::array<RhiSubmissionToken, RhiQueueTypeCount> tokens{};
		        const std::size_t tokenCount = generation.LastUse.CopyTokens(tokens);
		        for (std::size_t tokenIndex = 0; tokenIndex < tokenCount; ++tokenIndex)
		        {
			        if (!m_deviceServices.IsSubmissionComplete(tokens[tokenIndex]))
			        {
				        return false;
			        }
		        }
		        ShutdownProviders(generation.Upscaler, generation.RayReconstruction);
		        return true;
	        }),
	    m_retiredGenerations.end());
}

void RendererImageProviderStack::ResetHistory() noexcept
{
	m_bResetHistoryPending = true;
}

static ImageProviderFrameInput BuildImageProviderFrameInput(const RenderView& view, const RenderFrameIdentity& identity, bool bResetHistoryPending) noexcept
{
	return ImageProviderFrameInput{
	    .RenderExtent = view.renderExtent,
	    .OutputExtent = view.outputExtent,
	    .FrameId = identity.FrameId,
	    .ProviderGeneration = identity.ImageProviderGeneration,
	    .Camera = view.cameraUniform,
	    .Temporal = view.temporalUniform,
	    .bResetHistory = view.temporalUniform.HistoryValid == 0u || bResetHistoryPending};
}

void RendererImageProviderStack::SetupFrame(const RenderView& view, const RenderFrameIdentity& identity)
{
	ImageProviderFrameInput providerInput = BuildImageProviderFrameInput(view, identity, m_bResetHistoryPending);

	m_bResetHistoryPending = false;

	if (m_upscaler != nullptr)
	{
		m_upscaler->SetupFrame(providerInput);
	}

	if (ShouldUseRayReconstruction(view.viewMode))
	{
		SetupRayReconstructionFrame(providerInput);
	}
}

void RendererImageProviderStack::SetupRayReconstructionFrame(ImageProviderFrameInput& input)
{
	input.OutputExtent = input.RenderExtent;
	m_rayReconstruction->SetDenoisingExtent(input.RenderExtent);
	m_rayReconstruction->SetupFrame(input);
}

RenderViewportExtent RendererImageProviderStack::ResolveRenderExtent(RenderViewportExtent outputExtent) noexcept
{
	if (m_upscaler != nullptr)
	{
		return m_upscaler->ResolveRenderExtent(outputExtent);
	}
	return outputExtent;
}

ImageProviderGraphKey RendererImageProviderStack::GetFrameGraphKey() const noexcept
{
	return ImageProviderGraphKey{.UpscalerProvider = GetUpscalerProviderSelectionKey(), .RayReconstructionMode = static_cast<std::uint32_t>(CVarRayReconstructionMode.Get())};
}
