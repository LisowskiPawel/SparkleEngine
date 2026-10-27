#include "PCH.h"

#include "D3D12/Capture/D3D12CaptureReadback.h"

#include "Capture/RhiCaptureFormat.h"
#include "D3D12/D3D12TypeConversions.h"
#include "D3D12/Device/D3D12Rhi.h"

#include <cstring>

static void RecordTransition(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource, ResourceState before, ResourceState after) noexcept
{
	if (commandList == nullptr || resource == nullptr || before == after)
	{
		return;
	}

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = resource;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12TypeConversions::ToResourceStates(before);
	barrier.Transition.StateAfter = D3D12TypeConversions::ToResourceStates(after);
	commandList->ResourceBarrier(1, &barrier);
}

std::unique_ptr<D3D12CaptureService::PendingReadback> D3D12CaptureService::PrepareReadback(const RhiTextureCaptureRequest& request) noexcept
{
	ID3D12Device* const device = m_rhi.GetDevice().Get();
	ID3D12Resource* const sourceResource = static_cast<ID3D12Resource*>(request.Resource.Value);
	if (device == nullptr || sourceResource == nullptr)
	{
		return {};
	}

	const D3D12_RESOURCE_DESC sourceDesc = sourceResource->GetDesc();
	if (sourceDesc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || sourceDesc.Width == 0 || sourceDesc.Height == 0 || !IsRhiCaptureFormatSupported(request.SourceFormat)
	    || sourceDesc.Format != D3D12TypeConversions::ToDxgiFormat(request.SourceFormat))
	{
		return {};
	}

	auto pending = std::make_unique<PendingReadback>();
	pending->Ticket = RhiCaptureTicket{m_nextTicket++};
	pending->Request = request;
	UINT rowCount = 0;
	UINT64 rowSizeInBytes = 0;
	device->GetCopyableFootprints(&sourceDesc, 0, 1, 0, &pending->Footprint, &rowCount, &rowSizeInBytes, &pending->TotalBytes);
	if (pending->TotalBytes == 0 || rowCount == 0)
	{
		return {};
	}

	return pending;
}

bool D3D12CaptureService::AllocateReadbackBuffer(PendingReadback& pending) noexcept
{
	ID3D12Device* const device = m_rhi.GetDevice().Get();
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_READBACK;
	heapProperties.CreationNodeMask = 1;
	heapProperties.VisibleNodeMask = 1;
	D3D12_RESOURCE_DESC readbackDesc{};
	readbackDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	readbackDesc.Width = pending.TotalBytes;
	readbackDesc.Height = 1;
	readbackDesc.DepthOrArraySize = 1;
	readbackDesc.MipLevels = 1;
	readbackDesc.SampleDesc.Count = 1;
	readbackDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	return SUCCEEDED(device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &readbackDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&pending.Buffer)));
}

bool D3D12CaptureService::BeginReadbackCommands(PendingReadback& pending) noexcept
{
	ID3D12Device* const device = m_rhi.GetDevice().Get();

	return SUCCEEDED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pending.CommandAllocator)))
	    && SUCCEEDED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pending.CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&pending.CommandList)));
}

bool D3D12CaptureService::RecordReadbackCopy(PendingReadback& pending) noexcept
{
	const RhiTextureCaptureRequest& request = pending.Request;
	ID3D12Resource* const sourceResource = static_cast<ID3D12Resource*>(request.Resource.Value);
	RecordTransition(pending.CommandList.Get(), sourceResource, request.SourceState, ResourceState::CopySource);
	D3D12_TEXTURE_COPY_LOCATION sourceLocation{};
	sourceLocation.pResource = sourceResource;
	sourceLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	D3D12_TEXTURE_COPY_LOCATION destinationLocation{};
	destinationLocation.pResource = pending.Buffer.Get();
	destinationLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	destinationLocation.PlacedFootprint = pending.Footprint;
	pending.CommandList->CopyTextureRegion(&destinationLocation, 0, 0, 0, &sourceLocation, nullptr);
	RecordTransition(pending.CommandList.Get(), sourceResource, ResourceState::CopySource, request.SourceState);
	return SUCCEEDED(pending.CommandList->Close());
}

bool D3D12CaptureService::SubmitReadback(PendingReadback& pending) noexcept
{
	ID3D12CommandList* const commandLists[] = {pending.CommandList.Get()};
	pending.Submission = m_rhi.SubmitCommandLists(ERhiQueueType::Graphics, commandLists);
	return pending.Submission.IsValid();
}

void D3D12CaptureService::ReadPixels(PendingReadback& pending, RhiCaptureReadback& readback) noexcept
{
	void* mappedData = nullptr;
	const D3D12_RANGE readRange{0, static_cast<SIZE_T>(pending.TotalBytes)};
	if (FAILED(pending.Buffer->Map(0, &readRange, &mappedData)) || mappedData == nullptr)
	{
		readback.Result = RhiCaptureResult{
		    .Status = ERhiCaptureStatus::Failed,
		    .BackendApi = ERhiBackendApi::D3D12,
		    .FrameId = pending.Request.FrameId,
		    .FailureReason = "D3D12 texture readback could not be mapped"};

		return;
	}

	readback.Result = RhiCaptureResult{.Status = ERhiCaptureStatus::Succeeded, .BackendApi = ERhiBackendApi::D3D12, .FrameId = pending.Request.FrameId};
	readback.Width = pending.Footprint.Footprint.Width;
	readback.Height = pending.Footprint.Footprint.Height;
	readback.RowPitch = pending.Footprint.Footprint.RowPitch;
	readback.Format = pending.Request.SourceFormat;
	const std::size_t byteCount = static_cast<std::size_t>(readback.RowPitch) * readback.Height;
	readback.Pixels.resize(byteCount);
	const std::byte* source = static_cast<const std::byte*>(mappedData) + pending.Footprint.Offset;
	std::memcpy(readback.Pixels.data(), source, byteCount);
	const D3D12_RANGE writeRange{0, 0};
	pending.Buffer->Unmap(0, &writeRange);
}

bool D3D12CaptureService::IsReadbackComplete(const PendingReadback& pending) const noexcept
{
	return m_rhi.IsSubmissionComplete(pending.Submission);
}

void D3D12CaptureService::WaitForReadback(const PendingReadback& pending) noexcept
{
	m_rhi.WaitForSubmission(pending.Submission);
}
