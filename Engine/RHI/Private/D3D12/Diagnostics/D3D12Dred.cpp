#include "PCH.h"
#include "D3D12/Diagnostics/D3D12DebugLayer.h"

#if ENGINE_GPU_VALIDATION || SPARKLE_WITH_GPU_CRASH_DEBUGGING

  #include "Core/Public/Formatting/HexFormat.h"

  #include <format>
  #include <cstring>
  #include <cwchar>

namespace D3D12DredDiagnostics
{
	constexpr std::size_t kMaxBreadcrumbNodes = 16;
	constexpr std::size_t kMaxAllocationNodes = 8;

	std::string NarrowString(const wchar_t* text)
	{
		if (text == nullptr || text[0] == L'\0')
		{
			return {};
		}

		const int length = static_cast<int>(wcsnlen(text, 256));
		const int requiredSize = WideCharToMultiByte(CP_UTF8, 0, text, length, nullptr, 0, nullptr, nullptr);
		if (requiredSize == 0)
		{
			return {};
		}

		std::string result(static_cast<std::size_t>(requiredSize), '\0');
		WideCharToMultiByte(CP_UTF8, 0, text, length, result.data(), requiredSize, nullptr, nullptr);
		if (length == 256)
		{
			result += " [truncated]";
		}
		return result;
	}

	std::string ResolveDebugName(const char* narrowName, const wchar_t* wideName, std::string_view fallback)
	{
		if (narrowName != nullptr && narrowName[0] != '\0')
		{
			const auto length = strnlen(narrowName, 256);
			return std::string(narrowName, length) + (length == 256 ? " [truncated]" : "");
		}

		std::string convertedWideName = NarrowString(wideName);
		if (!convertedWideName.empty())
		{
			return convertedWideName;
		}

		return std::string(fallback);
	}

	void AppendDiagnosticMessage(std::deque<RhiDiagnosticMessage>& messages, ERhiDiagnosticMessageSeverity severity, ERhiDiagnosticMessageCategory category, std::string text)
	{
		messages.push_back(RhiDiagnosticMessage{.Severity = severity, .Category = category, .Text = std::move(text)});
	}

	std::string_view GetBreadcrumbOperationName(D3D12_AUTO_BREADCRUMB_OP op) noexcept
	{
		switch (op)
		{
			case D3D12_AUTO_BREADCRUMB_OP_SETMARKER:
				return "SetMarker";
			case D3D12_AUTO_BREADCRUMB_OP_BEGINEVENT:
				return "BeginEvent";
			case D3D12_AUTO_BREADCRUMB_OP_ENDEVENT:
				return "EndEvent";
			case D3D12_AUTO_BREADCRUMB_OP_DRAWINSTANCED:
				return "DrawInstanced";
			case D3D12_AUTO_BREADCRUMB_OP_DRAWINDEXEDINSTANCED:
				return "DrawIndexedInstanced";
			case D3D12_AUTO_BREADCRUMB_OP_DISPATCH:
				return "Dispatch";
			case D3D12_AUTO_BREADCRUMB_OP_COPYBUFFERREGION:
				return "CopyBufferRegion";
			case D3D12_AUTO_BREADCRUMB_OP_COPYTEXTUREREGION:
				return "CopyTextureRegion";
			case D3D12_AUTO_BREADCRUMB_OP_COPYRESOURCE:
				return "CopyResource";
			case D3D12_AUTO_BREADCRUMB_OP_CLEARRENDERTARGETVIEW:
				return "ClearRenderTargetView";
			case D3D12_AUTO_BREADCRUMB_OP_CLEARDEPTHSTENCILVIEW:
				return "ClearDepthStencilView";
			case D3D12_AUTO_BREADCRUMB_OP_RESOURCEBARRIER:
				return "ResourceBarrier";
			case D3D12_AUTO_BREADCRUMB_OP_BARRIER:
				return "Barrier";
			case D3D12_AUTO_BREADCRUMB_OP_RESOLVEQUERYDATA:
				return "ResolveQueryData";
			case D3D12_AUTO_BREADCRUMB_OP_BEGINSUBMISSION:
				return "BeginSubmission";
			case D3D12_AUTO_BREADCRUMB_OP_ENDSUBMISSION:
				return "EndSubmission";
			default:
				return "Other";
		}
	}

	std::string_view GetAllocationTypeName(D3D12_DRED_ALLOCATION_TYPE allocationType) noexcept
	{
		switch (allocationType)
		{
			case D3D12_DRED_ALLOCATION_TYPE_COMMAND_QUEUE:
				return "CommandQueue";
			case D3D12_DRED_ALLOCATION_TYPE_COMMAND_ALLOCATOR:
				return "CommandAllocator";
			case D3D12_DRED_ALLOCATION_TYPE_PIPELINE_STATE:
				return "PipelineState";
			case D3D12_DRED_ALLOCATION_TYPE_COMMAND_LIST:
				return "CommandList";
			case D3D12_DRED_ALLOCATION_TYPE_FENCE:
				return "Fence";
			case D3D12_DRED_ALLOCATION_TYPE_DESCRIPTOR_HEAP:
				return "DescriptorHeap";
			case D3D12_DRED_ALLOCATION_TYPE_HEAP:
				return "Heap";
			case D3D12_DRED_ALLOCATION_TYPE_RESOURCE:
				return "Resource";
			case D3D12_DRED_ALLOCATION_TYPE_QUERY_HEAP:
				return "QueryHeap";
			default:
				return "Other";
		}
	}

	std::string ResolveBreadcrumbContext(const D3D12_AUTO_BREADCRUMB_NODE1& node, UINT completedOperations)
	{
		std::string activeContext;
		for (UINT contextIndex = 0; node.pBreadcrumbContexts != nullptr && contextIndex < (std::min) (node.BreadcrumbContextsCount, 128u); ++contextIndex)
		{
			const D3D12_DRED_BREADCRUMB_CONTEXT& context = node.pBreadcrumbContexts[contextIndex];
			if (context.BreadcrumbIndex > completedOperations)
			{
				break;
			}

			std::string convertedContext = NarrowString(context.pContextString);
			if (!convertedContext.empty())
			{
				activeContext = std::move(convertedContext);
			}
		}

		if (node.BreadcrumbContextsCount > 128)
		{
			activeContext += " [contexts truncated]";
		}
		return activeContext;
	}

	void AppendBreadcrumbMessages(std::deque<RhiDiagnosticMessage>& messages, const D3D12_AUTO_BREADCRUMB_NODE1* node)
	{
		std::size_t reportedNodes = 0;
		const D3D12_AUTO_BREADCRUMB_NODE1* currentNode = node;
		for (; currentNode != nullptr && reportedNodes < kMaxBreadcrumbNodes; currentNode = currentNode->pNext, ++reportedNodes)
		{
			const UINT completedOperations = currentNode->pLastBreadcrumbValue != nullptr ? *currentNode->pLastBreadcrumbValue : 0u;
			const UINT historyIndex = completedOperations > 0 && currentNode->BreadcrumbCount > 0 ? (std::min) (completedOperations, currentNode->BreadcrumbCount) - 1 : 0u;

			const D3D12_AUTO_BREADCRUMB_OP lastOperation = currentNode->pCommandHistory != nullptr && currentNode->BreadcrumbCount > 0 ? currentNode->pCommandHistory[historyIndex]
			                                                                                                                           : D3D12_AUTO_BREADCRUMB_OP_BEGINSUBMISSION;

			const std::string commandListName = ResolveDebugName(currentNode->pCommandListDebugNameA, currentNode->pCommandListDebugNameW, "UnnamedCommandList");
			const std::string commandQueueName = ResolveDebugName(currentNode->pCommandQueueDebugNameA, currentNode->pCommandQueueDebugNameW, "UnnamedCommandQueue");
			const std::string breadcrumbContext = ResolveBreadcrumbContext(*currentNode, completedOperations);

			std::string message = std::format(
			    "DRED breadcrumb queue='{}' commandList='{}' completedOps={}/{} lastOp='{}'",
			    commandQueueName,
			    commandListName,
			    completedOperations,
			    currentNode->BreadcrumbCount,
			    GetBreadcrumbOperationName(lastOperation));

			if (!breadcrumbContext.empty())
			{
				message += std::format(" context='{}'", breadcrumbContext);
			}

			AppendDiagnosticMessage(messages, ERhiDiagnosticMessageSeverity::Error, ERhiDiagnosticMessageCategory::Driver, std::move(message));
		}

		if (currentNode != nullptr)
		{
			AppendDiagnosticMessage(
			    messages,
			    ERhiDiagnosticMessageSeverity::Warning,
			    ERhiDiagnosticMessageCategory::Driver,
			    "DRED breadcrumb list truncated after 16 nodes; remaining count unavailable.");
		}
	}

	template <typename TAllocationNode> void AppendAllocationMessages(std::deque<RhiDiagnosticMessage>& messages, const TAllocationNode* node, std::string_view allocationListName)
	{
		std::size_t reportedAllocations = 0;
		const TAllocationNode* currentNode = node;
		for (; currentNode != nullptr && reportedAllocations < kMaxAllocationNodes; currentNode = currentNode->pNext, ++reportedAllocations)
		{
			AppendDiagnosticMessage(
			    messages,
			    ERhiDiagnosticMessageSeverity::Error,
			    ERhiDiagnosticMessageCategory::Driver,
			    std::format(
			        "DRED {} allocation type='{}' name='{}'",
			        allocationListName,
			        GetAllocationTypeName(currentNode->AllocationType),
			        ResolveDebugName(currentNode->ObjectNameA, currentNode->ObjectNameW, "UnnamedObject")));
		}

		if (currentNode != nullptr)
		{
			AppendDiagnosticMessage(
			    messages,
			    ERhiDiagnosticMessageSeverity::Warning,
			    ERhiDiagnosticMessageCategory::Driver,
			    std::format("DRED {} allocation list truncated after 8 nodes; remaining count unavailable.", allocationListName));
		}
	}
}

using namespace D3D12DredDiagnostics;

void D3D12DebugLayer::CollectCrashDiagnostics(ID3D12Device* device)
{
	if (device == nullptr || !m_supportsCrashDiagnostics)
	{
		return;
	}

	const HRESULT deviceRemovedReason = device->GetDeviceRemovedReason();
	if (SUCCEEDED(deviceRemovedReason))
	{
		return;
	}

	DrainStoredMessages();

	AppendDiagnosticMessage(
	    m_messages,
	    ERhiDiagnosticMessageSeverity::Fatal,
	    ERhiDiagnosticMessageCategory::Driver,
	    std::format("D3D12 device removal detected with HRESULT {}", Formatting::FormatPrefixedHexUInt32(static_cast<std::uint32_t>(deviceRemovedReason))));

	ComPtr<ID3D12DeviceRemovedExtendedData1> dred1;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(dred1.ReleaseAndGetAddressOf()))))
	{
		D3D12_DRED_AUTO_BREADCRUMBS_OUTPUT1 autoBreadcrumbs{};
		if (SUCCEEDED(dred1->GetAutoBreadcrumbsOutput1(&autoBreadcrumbs)))
		{
			AppendBreadcrumbMessages(m_messages, autoBreadcrumbs.pHeadAutoBreadcrumbNode);
		}

		D3D12_DRED_PAGE_FAULT_OUTPUT1 pageFaultOutput{};
		if (SUCCEEDED(dred1->GetPageFaultAllocationOutput1(&pageFaultOutput)) && pageFaultOutput.PageFaultVA != 0)
		{
			AppendDiagnosticMessage(
			    m_messages,
			    ERhiDiagnosticMessageSeverity::Fatal,
			    ERhiDiagnosticMessageCategory::Driver,
			    std::format("DRED page fault at GPU VA {}", Formatting::FormatPrefixedHexUInt64(pageFaultOutput.PageFaultVA)));

			AppendAllocationMessages(m_messages, pageFaultOutput.pHeadExistingAllocationNode, "existing");
			AppendAllocationMessages(m_messages, pageFaultOutput.pHeadRecentFreedAllocationNode, "recent-freed");
		}
		return;
	}

	ComPtr<ID3D12DeviceRemovedExtendedData> dred;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(dred.ReleaseAndGetAddressOf()))))
	{
		D3D12_DRED_AUTO_BREADCRUMBS_OUTPUT autoBreadcrumbs{};
		if (SUCCEEDED(dred->GetAutoBreadcrumbsOutput(&autoBreadcrumbs)) && autoBreadcrumbs.pHeadAutoBreadcrumbNode != nullptr)
		{
			const D3D12_AUTO_BREADCRUMB_NODE* currentNode = autoBreadcrumbs.pHeadAutoBreadcrumbNode;
			std::size_t reportedNodes = 0;
			for (; currentNode != nullptr && reportedNodes < kMaxBreadcrumbNodes; currentNode = currentNode->pNext, ++reportedNodes)
			{
				const UINT completedOperations = currentNode->pLastBreadcrumbValue != nullptr ? *currentNode->pLastBreadcrumbValue : 0u;
				const UINT historyIndex = completedOperations > 0 && currentNode->BreadcrumbCount > 0 ? (std::min) (completedOperations, currentNode->BreadcrumbCount) - 1 : 0u;

				const D3D12_AUTO_BREADCRUMB_OP lastOperation = currentNode->pCommandHistory != nullptr && currentNode->BreadcrumbCount > 0 ? currentNode->pCommandHistory[historyIndex]
				                                                                                                                           : D3D12_AUTO_BREADCRUMB_OP_BEGINSUBMISSION;

				AppendDiagnosticMessage(
				    m_messages,
				    ERhiDiagnosticMessageSeverity::Error,
				    ERhiDiagnosticMessageCategory::Driver,
				    std::format(
				        "DRED breadcrumb queue='{}' commandList='{}' completedOps={}/{} lastOp='{}'",
				        ResolveDebugName(currentNode->pCommandQueueDebugNameA, currentNode->pCommandQueueDebugNameW, "UnnamedCommandQueue"),
				        ResolveDebugName(currentNode->pCommandListDebugNameA, currentNode->pCommandListDebugNameW, "UnnamedCommandList"),
				        completedOperations,
				        currentNode->BreadcrumbCount,
				        GetBreadcrumbOperationName(lastOperation)));
			}
		}

		D3D12_DRED_PAGE_FAULT_OUTPUT pageFaultOutput{};
		if (SUCCEEDED(dred->GetPageFaultAllocationOutput(&pageFaultOutput)) && pageFaultOutput.PageFaultVA != 0)
		{
			AppendDiagnosticMessage(
			    m_messages,
			    ERhiDiagnosticMessageSeverity::Fatal,
			    ERhiDiagnosticMessageCategory::Driver,
			    std::format("DRED page fault at GPU VA {}", Formatting::FormatPrefixedHexUInt64(pageFaultOutput.PageFaultVA)));

			AppendAllocationMessages(m_messages, pageFaultOutput.pHeadExistingAllocationNode, "existing");
			AppendAllocationMessages(m_messages, pageFaultOutput.pHeadRecentFreedAllocationNode, "recent-freed");
		}
	}
}

#endif
