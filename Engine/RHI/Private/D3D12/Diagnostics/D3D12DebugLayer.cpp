#include "PCH.h"
#include "D3D12/Diagnostics/D3D12DebugLayer.h"

#if ENGINE_GPU_VALIDATION || SPARKLE_WITH_GPU_CRASH_DEBUGGING

  #include "Core/Public/Formatting/HexFormat.h"

  #include <cstring>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_d3d12DiagnosticsLogger, "RHI.D3D12.Diagnostics");

namespace D3D12DebugLayerDiagnostics
{
	std::string FormatHRESULT(HRESULT hr)
	{
		return Formatting::FormatPrefixedHexUInt32(static_cast<std::uint32_t>(hr));
	}

	ERhiDiagnosticMessageSeverity ToDiagnosticSeverity(D3D12_MESSAGE_SEVERITY severity) noexcept
	{
		switch (severity)
		{
			case D3D12_MESSAGE_SEVERITY_CORRUPTION:
				return ERhiDiagnosticMessageSeverity::Fatal;
			case D3D12_MESSAGE_SEVERITY_ERROR:
				return ERhiDiagnosticMessageSeverity::Error;
			case D3D12_MESSAGE_SEVERITY_WARNING:
				return ERhiDiagnosticMessageSeverity::Warning;
			case D3D12_MESSAGE_SEVERITY_INFO:
				return ERhiDiagnosticMessageSeverity::Info;
			case D3D12_MESSAGE_SEVERITY_MESSAGE:
			default:
				return ERhiDiagnosticMessageSeverity::Verbose;
		}
	}

	ERhiDiagnosticMessageCategory ToDiagnosticCategory(D3D12_MESSAGE_CATEGORY category) noexcept
	{
		switch (category)
		{
			case D3D12_MESSAGE_CATEGORY_COMPILATION:
			case D3D12_MESSAGE_CATEGORY_SHADER:
				return ERhiDiagnosticMessageCategory::Shader;
			case D3D12_MESSAGE_CATEGORY_RESOURCE_MANIPULATION:
				return ERhiDiagnosticMessageCategory::ResourceLifetime;
			case D3D12_MESSAGE_CATEGORY_STATE_CREATION:
			case D3D12_MESSAGE_CATEGORY_STATE_SETTING:
			case D3D12_MESSAGE_CATEGORY_STATE_GETTING:
			case D3D12_MESSAGE_CATEGORY_EXECUTION:
				return ERhiDiagnosticMessageCategory::Validation;
			case D3D12_MESSAGE_CATEGORY_APPLICATION_DEFINED:
			case D3D12_MESSAGE_CATEGORY_MISCELLANEOUS:
			case D3D12_MESSAGE_CATEGORY_INITIALIZATION:
			case D3D12_MESSAGE_CATEGORY_CLEANUP:
			default:
				return ERhiDiagnosticMessageCategory::General;
		}
	}

}

using namespace D3D12DebugLayerDiagnostics;

D3D12DebugLayer::D3D12DebugLayer(bool enableValidation)
{
	InitDredSettings();
	if (enableValidation)
	{
		InitD3D12Debug();
		InitDXGIDebug();
	}
}

D3D12DebugLayer::~D3D12DebugLayer() noexcept
{
	m_messages.clear();
	m_infoQueue.Reset();
	m_dxgiDebug.Reset();
	m_d3d12Debug.Reset();
}

void D3D12DebugLayer::InitializeInfoQueue(ID3D12Device* device)
{
	if (!device)
	{
		m_infoQueue.Reset();
		m_messages.clear();
		SPDLOG_LOGGER_WARN(g_d3d12DiagnosticsLogger, "D3D12 debug messages unavailable: no device was available when attaching the info queue.");
		return;
	}

	const HRESULT infoQueueHr = device->QueryInterface(IID_PPV_ARGS(m_infoQueue.ReleaseAndGetAddressOf()));
	if (FAILED(infoQueueHr))
	{
		m_infoQueue.Reset();
		m_messages.clear();
		SPDLOG_LOGGER_WARN(g_d3d12DiagnosticsLogger, "D3D12 debug messages unavailable: ID3D12InfoQueue query failed with HRESULT {}.", FormatHRESULT(infoQueueHr));
		return;
	}

	ConfigureInfoQueue(device);
	ApplyInfoQueueFilters(device);
	ClearMessages();
}

bool D3D12DebugLayer::SupportsDebugMessages() const noexcept
{
	return m_infoQueue != nullptr;
}

bool D3D12DebugLayer::TryPopMessage(RhiDiagnosticMessage& outMessage)
{
	if (!SupportsDebugMessages() && m_messages.empty())
	{
		return false;
	}

	if (m_messages.empty())
	{
		DrainStoredMessages();
	}

	if (m_messages.empty())
	{
		return false;
	}

	outMessage = std::move(m_messages.front());
	m_messages.pop_front();
	return true;
}

void D3D12DebugLayer::ClearMessages() noexcept
{
	m_messages.clear();
	if (m_infoQueue != nullptr)
	{
		m_infoQueue->ClearStoredMessages();
	}
}

bool D3D12DebugLayer::SupportsLiveObjectReports() const noexcept
{
  #if ENGINE_REPORT_LIVE_OBJECTS
	return m_dxgiDebug != nullptr;
  #else
	return false;
  #endif
}

bool D3D12DebugLayer::SupportsCrashDiagnostics() const noexcept
{
	return m_supportsCrashDiagnostics;
}

void D3D12DebugLayer::ReportLiveObjects(ID3D12Device* device)
{
	ReportLiveDeviceObjects(device);
	ReportLiveDXGIObjects();
}

void D3D12DebugLayer::InitDredSettings() noexcept
{
	ComPtr<ID3D12DeviceRemovedExtendedDataSettings> dredSettings;
	const HRESULT dredHr = D3D12GetDebugInterface(IID_PPV_ARGS(dredSettings.ReleaseAndGetAddressOf()));
	if (FAILED(dredHr))
	{
		m_supportsCrashDiagnostics = false;
		SPDLOG_LOGGER_WARN(g_d3d12DiagnosticsLogger, "D3D12 crash diagnostics unavailable: DRED settings interface query failed with HRESULT {}.", FormatHRESULT(dredHr));
		return;
	}

	dredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
	dredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
	dredSettings->SetWatsonDumpEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);

	ComPtr<ID3D12DeviceRemovedExtendedDataSettings1> dredSettings1;
	if (SUCCEEDED(dredSettings->QueryInterface(IID_PPV_ARGS(dredSettings1.ReleaseAndGetAddressOf()))))
	{
		dredSettings1->SetBreadcrumbContextEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
	}

	m_supportsCrashDiagnostics = true;
}

void D3D12DebugLayer::InitD3D12Debug()
{
	CHECK(D3D12GetDebugInterface(IID_PPV_ARGS(m_d3d12Debug.ReleaseAndGetAddressOf())));
	m_d3d12Debug->EnableDebugLayer();
}

void D3D12DebugLayer::InitDXGIDebug()
{
	CHECK(DXGIGetDebugInterface1(0, IID_PPV_ARGS(m_dxgiDebug.ReleaseAndGetAddressOf())));
	m_dxgiDebug->EnableLeakTrackingForThread();
}

void D3D12DebugLayer::ConfigureInfoQueue(ID3D12Device* device)
{
	static_cast<void>(device);

	if (m_infoQueue != nullptr)
	{
		const BOOL debuggerAttached = IsDebuggerPresent();
		m_infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, debuggerAttached);
		m_infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, debuggerAttached);
		m_infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, debuggerAttached);
	}
}

void D3D12DebugLayer::ApplyInfoQueueFilters(ID3D12Device* device)
{
	if (m_infoQueue != nullptr)
	{
		D3D12_MESSAGE_ID disabledMessages[] = {static_cast<D3D12_MESSAGE_ID>(1424)};
		D3D12_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumIDs = static_cast<UINT>(std::size(disabledMessages));
		filter.DenyList.pIDList = disabledMessages;
		m_infoQueue->AddStorageFilterEntries(&filter);
	}
}

void D3D12DebugLayer::DrainStoredMessages()
{
	if (m_infoQueue == nullptr)
	{
		return;
	}

	const UINT64 numMessages = m_infoQueue->GetNumStoredMessagesAllowedByRetrievalFilter();
	bool truncated = numMessages > 256;
	for (UINT64 messageIndex = 0; messageIndex < (std::min) (numMessages, UINT64{256}); ++messageIndex)
	{
		SIZE_T messageLength = 0;
		if (FAILED(m_infoQueue->GetMessage(messageIndex, nullptr, &messageLength)) || messageLength == 0)
		{
			continue;
		}
		if (messageLength > 64u * 1024u)
		{
			truncated = true;
			continue;
		}

		std::vector<char> messageData(messageLength);
		auto* message = reinterpret_cast<D3D12_MESSAGE*>(messageData.data());
		if (SUCCEEDED(m_infoQueue->GetMessage(messageIndex, message, &messageLength)))
		{
			m_messages.push_back(
			    RhiDiagnosticMessage{
			        .Severity = ToDiagnosticSeverity(message->Severity),
			        .Category = ToDiagnosticCategory(message->Category),
			        .Text = message->pDescription != nullptr ? std::string(message->pDescription, strnlen(message->pDescription, messageLength)) : ""});
		}
	}
	if (truncated)
	{
		m_messages.push_back(
		    RhiDiagnosticMessage{
		        .Severity = ERhiDiagnosticMessageSeverity::Warning,
		        .Category = ERhiDiagnosticMessageCategory::Driver,
		        .Text = "D3D12 validation messages truncated to 256 entries of at most 64 KiB."});
	}

	m_infoQueue->ClearStoredMessages();
}

void D3D12DebugLayer::ReportLiveDeviceObjects(ID3D12Device* device)
{
  #if ENGINE_REPORT_LIVE_OBJECTS
	ComPtr<ID3D12DebugDevice> debugDevice;
	if (device && SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(debugDevice.ReleaseAndGetAddressOf()))))
	{
		OutputDebugStringW(L"D3D12 Live Device Objects (detail + summary):\n");
		debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_SUMMARY);
	}
  #endif
}

void D3D12DebugLayer::ReportLiveDXGIObjects()
{
  #if ENGINE_REPORT_LIVE_OBJECTS
	if (m_dxgiDebug)
	{
		OutputDebugStringW(L"DXGI Live Objects (all flags):\n");
		m_dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_ALL));
	}
  #endif
}
#endif
