#include "PCH.h"

#include "D3D12/Device/D3D12Rhi.h"
#include "D3D12/Diagnostics/D3D12DebugLayer.h"
#include "RHI/Public/Diagnostics/RhiDiagnostics.h"
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
  #include "D3D12/Diagnostics/GpuCrash/D3D12GpuCrash.h"
#endif

void D3D12Rhi::InitializeDebugLayer() noexcept
{
#if SPARKLE_WITH_GPU_CRASH_DEBUGGING
	m_debugLayer = D3D12GpuCrash::TakePreparedDred();
#endif
#if ENGINE_GPU_VALIDATION
	if (m_debugLayer == nullptr)
	{
		m_debugLayer = std::make_unique<D3D12DebugLayer>();
	}
#endif
}

void D3D12Rhi::InitializeDeviceDiagnostics() noexcept
{
#if ENGINE_GPU_VALIDATION
	m_debugLayer->InitializeInfoQueue(m_device.Get());
#endif
}

bool D3D12Rhi::IsValidationEnabled() const noexcept
{
#if ENGINE_GPU_VALIDATION
	return m_debugLayer != nullptr;
#else
	return false;
#endif
}

bool D3D12Rhi::SupportsDebugMessages() const noexcept
{
#if ENGINE_GPU_VALIDATION
	return m_debugLayer != nullptr && m_debugLayer->SupportsDebugMessages();
#else
	return false;
#endif
}

bool D3D12Rhi::TryPopDebugMessage(RhiDiagnosticMessage& outMessage) noexcept
{
#if ENGINE_GPU_VALIDATION
	return m_debugLayer != nullptr && m_debugLayer->TryPopMessage(outMessage);
#else
	static_cast<void>(outMessage);
	return false;
#endif
}

void D3D12Rhi::ClearDebugMessages() noexcept
{
#if ENGINE_GPU_VALIDATION
	if (m_debugLayer != nullptr)
	{
		m_debugLayer->ClearMessages();
	}
#endif
}

bool D3D12Rhi::SupportsLiveObjectReports() const noexcept
{
#if ENGINE_GPU_VALIDATION
	return m_debugLayer != nullptr && m_debugLayer->SupportsLiveObjectReports();
#else
	return false;
#endif
}

bool D3D12Rhi::SupportsCrashDiagnostics() const noexcept
{
#if ENGINE_GPU_VALIDATION || SPARKLE_WITH_GPU_CRASH_DEBUGGING
	return m_debugLayer != nullptr && m_debugLayer->SupportsCrashDiagnostics();
#else
	return false;
#endif
}

void D3D12Rhi::ReportLiveObjects() noexcept
{
#if ENGINE_GPU_VALIDATION
	if (m_debugLayer != nullptr)
	{
		m_debugLayer->ReportLiveObjects(m_device.Get());
	}
#endif
}

void D3D12Rhi::CollectCrashDiagnostics() noexcept
{
#if ENGINE_GPU_VALIDATION || SPARKLE_WITH_GPU_CRASH_DEBUGGING
	if (m_debugLayer != nullptr)
	{
		m_debugLayer->CollectCrashDiagnostics(m_device.Get());
	}
#endif
}
