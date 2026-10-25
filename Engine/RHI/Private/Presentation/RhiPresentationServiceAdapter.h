#pragma once

#include "Presentation/RhiPresentationService.h"

#include "Core/Public/Diagnostics/Logger.h"
#include "Core/Public/Diagnostics/Verify.h"

#include <utility>

template <typename Implementation> class RhiPresentationServiceAdapter final : public RhiPresentationService
{
public:
	template <typename... Args> explicit RhiPresentationServiceAdapter(Args&&... args) noexcept :
	    m_implementation(std::forward<Args>(args)...)
	{
	}

	Implementation& GetImplementation() noexcept { return m_implementation; }

	RhiViewport GetBackBufferViewport() const noexcept override { return m_implementation.GetBackBufferViewport(); }

	RhiRect GetBackBufferScissorRect() const noexcept override { return m_implementation.GetBackBufferScissorRect(); }

	RhiCpuDescriptorHandle GetBackBufferRenderTargetView() const noexcept override { return m_implementation.GetBackBufferRenderTargetView(); }

	RhiResourceHandle GetBackBufferResource() const noexcept override { return m_implementation.GetBackBufferResource(); }

	void BeginPresentRenderPass(RhiClearColorView clearColor) noexcept override
	{
		Begin();
		m_implementation.BeginPresentRenderPass(clearColor);
	}

	void BeginPresentOverlayPass() noexcept override
	{
		Begin();
		m_implementation.BeginPresentOverlayPass();
	}

	void EndPresentRenderPass() noexcept override
	{
		if (!m_rendering)
		{
			Fail("Present rendering ended without a matching begin.");
		}
		m_implementation.EndPresentRenderPass();
		m_rendering = false;
	}

	PixelFormat GetPresentColorFormat() const noexcept override { return m_implementation.GetPresentColorFormat(); }

private:
	void Begin() noexcept
	{
		if (m_rendering)
		{
			Fail("Present rendering began while another present render pass was active.");
		}
		m_rendering = true;
	}

	[[noreturn]] static void Fail(const char* message) noexcept
	{
		SPARKLE_DEFINE_LOG_CATEGORY_STATIC(logger, "RHI.Presentation");
		Diagnostics::Fatal(logger, __FILE__, __LINE__, message);
	}

	Implementation m_implementation;
	bool m_rendering = false;
};
