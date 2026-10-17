#pragma once

#include <d3d12.h>
#include <GFSDK_Aftermath.h>
#include "Diagnostics/GpuCrash/GpuCrashSession.h"
#include "Commands/RhiQueue.h"
#include <string_view>

class D3D12DebugLayer;

namespace D3D12GpuCrash
{
	std::unique_ptr<D3D12DebugLayer> TakePreparedDred() noexcept;
	void Initialize(ID3D12Device* device);
	void Shader(const D3D12_SHADER_BYTECODE& code) noexcept;
	void Resource(ID3D12Resource* resource) noexcept;
	void ObserveLoss(ID3D12Device* device, HRESULT result) noexcept;
	void ObserveLoss(ID3D12DeviceChild* source, HRESULT result) noexcept;
}

class D3D12GpuCrashContext final
{
public:
	D3D12GpuCrashContext(ID3D12GraphicsCommandList* commands, ERhiQueueType queue);
	~D3D12GpuCrashContext();
	D3D12GpuCrashContext(const D3D12GpuCrashContext&) = delete;
	D3D12GpuCrashContext& operator=(const D3D12GpuCrashContext&) = delete;
	void Begin(std::string_view label) noexcept;
	void End() noexcept;
	void Insert(std::string_view label) noexcept;
	void Reset() noexcept;

private:
	void Emit(const void* token) noexcept;
	GFSDK_Aftermath_ContextHandle m_context = nullptr;
	PFN_GFSDK_Aftermath_ReleaseContextHandle m_release = nullptr;
	PFN_GFSDK_Aftermath_SetEventMarker m_marker = nullptr;
	GpuCrashScopes m_scopes;
	std::uint64_t m_contextId = 0;
	std::uint64_t m_recordingId = 0;
	ERhiQueueType m_queue;
};
