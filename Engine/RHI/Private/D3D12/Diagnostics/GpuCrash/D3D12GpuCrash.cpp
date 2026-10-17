#include "PCH.h"
#include "D3D12/Diagnostics/GpuCrash/D3D12GpuCrash.h"

#include "D3D12/Diagnostics/D3D12DebugLayer.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Json/JsonWriter.h"

#include <wrl/client.h>
#include <format>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(LogD3D12GpuCrash, "RHI.D3D12.GpuCrash");

namespace D3D12GpuCrash
{
	D3D12DebugLayer* Dred = nullptr;
	std::unique_ptr<D3D12DebugLayer> PreparedDred;
	std::atomic<std::uint64_t> NextContext{1};
}

void PrepareD3D12GpuCrash()
{
	D3D12GpuCrash::PreparedDred = std::make_unique<D3D12DebugLayer>(ENGINE_GPU_VALIDATION != 0);
	D3D12GpuCrash::Dred = D3D12GpuCrash::PreparedDred.get();
	if (!D3D12GpuCrash::Dred->SupportsCrashDiagnostics())
	{
		throw Diagnostics::Error("Requested GPU crash diagnostics require pre-device DRED configuration.");
	}
}

std::unique_ptr<D3D12DebugLayer> D3D12GpuCrash::TakePreparedDred() noexcept
{
	return std::move(PreparedDred);
}

void D3D12GpuCrash::Initialize(ID3D12Device* device)
{
	if (auto* session = GpuCrashSession::Current())
	{
		const auto initialize = reinterpret_cast<PFN_GFSDK_Aftermath_DX12_Initialize>(session->RequireExport("GFSDK_Aftermath_DX12_Initialize"));

		constexpr std::uint32_t flags = GFSDK_Aftermath_FeatureFlags_EnableMarkers | GFSDK_Aftermath_FeatureFlags_EnableResourceTracking | GFSDK_Aftermath_FeatureFlags_GenerateShaderDebugInfo
		    | GFSDK_Aftermath_FeatureFlags_EnableShaderErrorReporting;

		if (initialize(GFSDK_Aftermath_Version_API, flags, device) != GFSDK_Aftermath_Result_Success)
		{
			Diagnostics::Fatal(LogD3D12GpuCrash, __FILE__, __LINE__, "Aftermath rejected the selected D3D12 device/driver; requested launch cannot continue.");
		}
		session->Ready();
	}
}

void D3D12GpuCrash::Shader(const D3D12_SHADER_BYTECODE& code) noexcept
{
	if (auto* session = GpuCrashSession::Current())
	{
		const auto hash = reinterpret_cast<PFN_GFSDK_Aftermath_GetShaderHash>(session->RequireExport("GFSDK_Aftermath_GetShaderHash"));
		const auto name = reinterpret_cast<PFN_GFSDK_Aftermath_GetShaderDebugName>(session->RequireExport("GFSDK_Aftermath_GetShaderDebugName"));
		GFSDK_Aftermath_ShaderBinaryHash id{};
		GFSDK_Aftermath_ShaderDebugName debug{};
		const bool hasHash = hash(GFSDK_Aftermath_Version_API, &code, &id) == GFSDK_Aftermath_Result_Success;
		const bool hasName = name(GFSDK_Aftermath_Version_API, &code, &debug) == GFSDK_Aftermath_Result_Success;
		session->ShaderIdentity({static_cast<const std::byte*>(code.pShaderBytecode), code.BytecodeLength}, id.hash, hasHash, hasName ? &debug : nullptr);
	}
}

void D3D12GpuCrash::Resource(ID3D12Resource* resource) noexcept
{
	if (auto* session = GpuCrashSession::Current(); session != nullptr && resource != nullptr)
	{
		const auto update = reinterpret_cast<PFN_GFSDK_Aftermath_DX12_UpdateResourceInfo>(session->RequireExport("GFSDK_Aftermath_DX12_UpdateResourceInfo"));
		if (update(resource) != GFSDK_Aftermath_Result_Success)
		{
			session->Incomplete();
		}
	}
}

void D3D12GpuCrash::ObserveLoss(ID3D12Device* device, HRESULT result) noexcept
{
	auto* session = GpuCrashSession::Current();
	if (session == nullptr || device == nullptr || SUCCEEDED(result) || SUCCEEDED(device->GetDeviceRemovedReason()) || !session->BeginLoss(result))
	{
		return;
	}
	try
	{
		Dred->CollectCrashDiagnostics(device);
		Json::ObjectWriter json;
		json.WriteRaw("frame", "null");
		json.WriteString("resourceAliases", "ambiguous; native allocation candidates only");
		std::string messages = "[";
		RhiDiagnosticMessage message;
		while (Dred->TryPopMessage(message))
		{
			if (messages.size() > 1)
			{
				messages += ',';
			}
			messages += Json::QuoteString(message.Text);
		}
		messages += ']';
		json.WriteRaw("dred", messages);
		const auto text = json.Finish();
		session->Enqueue({RhiGpuCrashArtifactKind::NativeDiagnostics, 0, 0}, std::as_bytes(std::span(text)));
	}
	catch (...)
	{
		session->Incomplete();
	}
	session->FinishLoss(result);
}

void D3D12GpuCrash::ObserveLoss(ID3D12DeviceChild* source, HRESULT result) noexcept
{
	if (FAILED(result) && GpuCrashSession::Current() != nullptr && source != nullptr)
	{
		Microsoft::WRL::ComPtr<ID3D12Device> device;
		if (SUCCEEDED(source->GetDevice(IID_PPV_ARGS(&device))))
		{
			ObserveLoss(device.Get(), result);
		}
	}
}

D3D12GpuCrashContext::D3D12GpuCrashContext(ID3D12GraphicsCommandList* commands, ERhiQueueType queue) :
    m_contextId(D3D12GpuCrash::NextContext.fetch_add(1)),
    m_queue(queue)
{
	if (auto* session = GpuCrashSession::Current())
	{
		m_release = reinterpret_cast<PFN_GFSDK_Aftermath_ReleaseContextHandle>(session->RequireExport("GFSDK_Aftermath_ReleaseContextHandle"));
		m_marker = reinterpret_cast<PFN_GFSDK_Aftermath_SetEventMarker>(session->RequireExport("GFSDK_Aftermath_SetEventMarker"));
		const auto create = reinterpret_cast<PFN_GFSDK_Aftermath_DX12_CreateContextHandle>(session->RequireExport("GFSDK_Aftermath_DX12_CreateContextHandle"));
		if (create(commands, &m_context) != GFSDK_Aftermath_Result_Success)
		{
			Diagnostics::Fatal(LogD3D12GpuCrash, __FILE__, __LINE__, "Aftermath command-list context creation failed.");
		}
	}
}

D3D12GpuCrashContext::~D3D12GpuCrashContext()
{
	if (m_context != nullptr && m_release(m_context) != GFSDK_Aftermath_Result_Success)
	{
		GpuCrashSession::Current()->Incomplete();
	}
}

void D3D12GpuCrashContext::Emit(const void* token) noexcept
{
	if (m_context != nullptr && token != nullptr)
	{
		try
		{
			const auto scope = GpuCrashSession::Current()->MarkerText(token);
			std::array<char, 128> text{};
			const auto written = std::format_to_n(text.data(), text.size() - 1, "q={} c={} r={} {}", static_cast<unsigned>(m_queue), m_contextId, m_recordingId, scope);
			if (written.size >= text.size())
			{
				GpuCrashSession::Current()->Incomplete();
			}
			const auto size = std::min<std::size_t>(written.size, text.size() - 1);
			if (m_marker(m_context, text.data(), static_cast<std::uint32_t>(size)) != GFSDK_Aftermath_Result_Success)
			{
				GpuCrashSession::Current()->Incomplete();
			}
		}
		catch (...)
		{
			GpuCrashSession::Current()->Incomplete();
		}
	}
}

void D3D12GpuCrashContext::Begin(std::string_view label) noexcept
{
	Emit(m_scopes.Begin(label));
}

void D3D12GpuCrashContext::End() noexcept
{
	Emit(m_scopes.End());
}

void D3D12GpuCrashContext::Insert(std::string_view label) noexcept
{
	Emit(m_scopes.Insert(label));
}

void D3D12GpuCrashContext::Reset() noexcept
{
	++m_recordingId;
	m_scopes.Reset();
}
