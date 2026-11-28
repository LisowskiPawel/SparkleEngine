#include "PCH.h"

#include "D3D12/Capture/D3D12TextureReadback.h"

#include "Capture/RhiCaptureFormat.h"
#include "D3D12/D3D12TypeConversions.h"
#include "D3D12/Device/D3D12Rhi.h"
#include "D3D12/Memory/D3D12GpuMemoryAllocator.h"

#include <cstring>

D3D12TextureReadback::D3D12TextureReadback(D3D12Rhi& rhi, D3D12GpuMemoryAllocator& memoryAllocator) noexcept :
    m_rhi(rhi),
    m_memoryAllocator(memoryAllocator)
{
}

D3D12TextureReadback::~D3D12TextureReadback() noexcept = default;

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

std::unique_ptr<D3D12TextureReadback::PendingReadback> D3D12TextureReadback::PrepareReadback(const RhiTextureCaptureRequest& request) noexcept
{
	ID3D12Device* const device = m_rhi.GetDevice().Get();
	ID3D12Resource* const sourceResource = static_cast<ID3D12Resource*>(request.Resource.Value);
	if (device == nullptr || sourceResource == nullptr)
	{
		return {};
	}

	const D3D12_RESOURCE_DESC sourceDesc = sourceResource->GetDesc();
	const bool bIncompatibleCaptureSourceWidth = sourceDesc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || sourceDesc.Width == 0;
	const bool bIncompatibleCaptureSourceHeight = bIncompatibleCaptureSourceWidth || (sourceDesc.Height == 0);
	const bool bCaptureSourceSourceFormat = bIncompatibleCaptureSourceHeight || (!IsRhiCaptureFormatSupported(request.SourceFormat));
	const bool bIncompatibleCaptureSource = bCaptureSourceSourceFormat || (sourceDesc.Format != D3D12TypeConversions::ToDxgiFormat(request.SourceFormat));

	if (bIncompatibleCaptureSource)
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

bool D3D12TextureReadback::AllocateReadbackBuffer(PendingReadback& pending) noexcept
{
	D3D12_RESOURCE_DESC readbackDesc{};
	readbackDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	readbackDesc.Width = pending.TotalBytes;
	readbackDesc.Height = 1;
	readbackDesc.DepthOrArraySize = 1;
	readbackDesc.MipLevels = 1;
	readbackDesc.SampleDesc.Count = 1;
	readbackDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	pending.Buffer = m_memoryAllocator.CreateBuffer(readbackDesc, D3D12_RESOURCE_STATE_COPY_DEST, RhiMemoryCategory::Readback, RhiMemoryResidencyClass::HostReadback, L"Texture capture readback");
	return pending.Buffer != nullptr;
}

bool D3D12TextureReadback::BeginReadbackCommands(PendingReadback& pending) noexcept
{
	ID3D12Device* const device = m_rhi.GetDevice().Get();

	const HRESULT allocatorResult = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pending.CommandAllocator));
	if (FAILED(allocatorResult))
	{
		return false;
	}

	const HRESULT commandListResult = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pending.CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&pending.CommandList));
	return SUCCEEDED(commandListResult);
}

bool D3D12TextureReadback::RecordReadbackCopy(PendingReadback& pending) noexcept
{
	const RhiTextureCaptureRequest& request = pending.Request;
	ID3D12Resource* const sourceResource = static_cast<ID3D12Resource*>(request.Resource.Value);
	RecordTransition(pending.CommandList.Get(), sourceResource, request.SourceState, ResourceState::CopySource);
	D3D12_TEXTURE_COPY_LOCATION sourceLocation{};
	sourceLocation.pResource = sourceResource;
	sourceLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	D3D12_TEXTURE_COPY_LOCATION destinationLocation{};
	destinationLocation.pResource = pending.Buffer->Resource.Get();
	destinationLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	destinationLocation.PlacedFootprint = pending.Footprint;
	pending.CommandList->CopyTextureRegion(&destinationLocation, 0, 0, 0, &sourceLocation, nullptr);
	RecordTransition(pending.CommandList.Get(), sourceResource, ResourceState::CopySource, request.SourceState);
	return SUCCEEDED(pending.CommandList->Close());
}

bool D3D12TextureReadback::SubmitReadback(PendingReadback& pending) noexcept
{
	D3D12RecordingResourceUseToken sourceUse = m_memoryAllocator.RetainRecordingResource(pending.Request.Resource);
	if (!sourceUse)
	{
		sourceUse = m_memoryAllocator.RetainCoordinatorRecordingResource(pending.Request.Resource);
	}

	ID3D12CommandList* const commandLists[] = {pending.CommandList.Get()};
	pending.Submission = m_rhi.SubmitCommandLists(ERhiQueueType::Graphics, commandLists);
	m_memoryAllocator.ReleaseRecordingResource(sourceUse, pending.Submission);
	return pending.Submission.IsValid();
}

void D3D12TextureReadback::ReadPixels(PendingReadback& pending, RhiCaptureReadback& readback) noexcept
{
	void* mappedData = nullptr;
	const D3D12_RANGE readRange{0, static_cast<SIZE_T>(pending.TotalBytes)};
	const HRESULT mapResult = pending.Buffer->Resource->Map(0, &readRange, &mappedData);
	if (FAILED(mapResult) || mappedData == nullptr)
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
	pending.Buffer->Resource->Unmap(0, &writeRange);
}

bool D3D12TextureReadback::IsReadbackComplete(const PendingReadback& pending) const noexcept
{
	return m_rhi.IsSubmissionComplete(pending.Submission);
}

void D3D12TextureReadback::WaitForReadback(const PendingReadback& pending) noexcept
{
	m_rhi.WaitForSubmission(pending.Submission);
}
