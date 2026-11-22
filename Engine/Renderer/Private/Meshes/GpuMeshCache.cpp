#include "PCH.h"
#include "Meshes/GpuMeshCache.h"

#include "Core/Public/Diagnostics/Verify.h"
#include "Meshes/GpuMeshPreparation.h"
#include "RHI/Public/Commands/RhiCommandSubmissionService.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"
#include <TaskScheduler_c.h>

#include <algorithm>
#include <utility>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_gpuMeshCacheLogger, "Renderer.GpuMeshCache");

GpuMeshCache::GpuMeshCache(RenderHardwareInterface& renderHardwareInterface, RhiCommandSubmissionService& submissions, enkiTaskScheduler* cpu) :
    m_renderHardwareInterface(renderHardwareInterface),
    m_submissions(submissions),
    m_cpu(cpu)
{
}

GpuMeshCache::~GpuMeshCache() noexcept
{
	SettlePreparations();

	m_requests.clear();
	m_sourceHandles.clear();
	m_handles.clear();
	m_cache.clear();
}

void GpuMeshCache::SettlePreparations() noexcept
{
	m_stop.request_stop();
	bool bRegistered = false;
	for (auto& [key, request] : m_requests)
	{
		if (!request.Preparation)
		{
			continue;
		}
		if (!enkiIsPinnedTaskComplete(m_cpu, request.Preparation))
		{
			if (enkiGetThreadNum(m_cpu) == ENKI_NO_THREAD_NUM)
			{
				if (!enkiRegisterExternalTaskThread(m_cpu))
				{
					Diagnostics::Fatal(g_gpuMeshCacheLogger, __FILE__, __LINE__, "GPU mesh teardown could not acquire its reserved native CPU registration.");
				}
				bRegistered = true;
			}
			if (!bRegistered && enkiGetThreadNum(m_cpu) != 0)
			{
				Diagnostics::Fatal(g_gpuMeshCacheLogger, __FILE__, __LINE__, "GPU mesh teardown requires its creator or the detached render owner.");
			}
			enkiWaitForPinnedTask(m_cpu, request.Preparation);
		}
		enkiDeletePinnedTask(m_cpu, request.Preparation);
	}
	if (bRegistered)
	{
		enkiDeRegisterExternalTaskThread(m_cpu);
	}
}

GpuMeshHandle GpuMeshCache::Request(const ImmutableRenderMeshHandle& mesh)
{
	if (!mesh.IsValid())
	{
		Diagnostics::Fatal(g_gpuMeshCacheLogger, __FILE__, __LINE__, "GPU mesh cache received an invalid mesh handle.");
	}

	const CacheKey key{mesh.GetAssetId(), mesh.GetGeneration()};
	const auto active = m_cache.find(key);
	if (active != m_cache.end())
	{
		return active->second.Mesh->GetHandle();
	}

	MeshRequest* const existingRequest = FindRequest(key);
	if (existingRequest != nullptr)
	{
		if (!existingRequest->bWanted)
		{
			return {};
		}
		existingRequest->bWanted = true;
		return existingRequest->Handle;
	}

	const std::size_t maximumRequestCount = m_residency.GetBudget().MaximumRequestBacklog;
	if (m_requests.size() >= maximumRequestCount)
	{
		RemoveTerminalRequests();
	}
	if (m_requests.size() >= maximumRequestCount || !m_residency.HasRequestCapacity())
	{
		return {};
	}

	auto prepared = std::make_unique<GpuMeshPreparedData>();
	const std::optional<AssetGenerationHandle> generation = m_residency.BeginGeneration(key.first, key.second);
	if (!generation)
	{
		return {};
	}

	const GpuMeshHandle handle = AllocateHandle();

	MeshRequest request;
	request.Key = key;
	request.Source = mesh;
	request.Handle = handle;
	request.Generation = *generation;
	request.Prepared = std::move(prepared);
	request.Stop = m_stop.get_token();
	try
	{
		if (!m_requests.emplace(key, std::move(request)).second)
		{
			Diagnostics::Fatal(g_gpuMeshCacheLogger, __FILE__, __LINE__, "GPU mesh request was admitted twice.");
		}
	}
	catch (...)
	{
		(void) m_residency.Cancel(*generation);
		throw;
	}
	LaunchPendingPreparations();
	return handle;
}

void GpuMeshCache::UploadReadyMeshes(RenderCommandList& commandList)
{
	ConsumeCompletedPreparations();
	LaunchPendingPreparations();

	for (auto& entry : m_requests)
	{
		MeshRequest& request = entry.second;
		const bool bUploadWantedPrepared = request.bWanted && request.Prepared != nullptr;
		const bool bUploadWanted = bUploadWantedPrepared && (request.Uploaded == nullptr);
		const bool bUploadReady = bUploadWanted && m_residency.GetState(request.Generation) == AssetResidencyState::ReadyForUpload;
		const bool bUploadStarted = bUploadReady && m_residency.BeginUpload(request.Generation);

		if (!bUploadStarted)
		{
			continue;
		}

		request.ResidentBytes = request.Prepared->GetResidentByteSize();
		auto gpuMesh = std::make_unique<GpuMesh>(request.Handle);
		gpuMesh->Upload(m_renderHardwareInterface, commandList, std::move(*request.Prepared));

		request.Prepared.reset();
		request.Uploaded = std::move(gpuMesh);
	}
}

void GpuMeshCache::RecordUploadSubmission(RhiSubmissionToken token) noexcept
{
	if (!token.IsValid())
	{
		for (const auto& entry : m_requests)
		{
			const MeshRequest& request = entry.second;
			if (request.Uploaded != nullptr && !request.bUploadSubmitted)
			{
				Diagnostics::Fatal(g_gpuMeshCacheLogger, __FILE__, __LINE__, "GPU mesh upload completed without a submission token.");
			}
		}
		return;
	}

	for (auto& entry : m_requests)
	{
		MeshRequest& request = entry.second;
		if (request.Uploaded != nullptr && !request.bUploadSubmitted)
		{
			if (!m_residency.RecordUploadSubmission(request.Generation, token, request.ResidentBytes))
			{
				Diagnostics::Fatal(g_gpuMeshCacheLogger, __FILE__, __LINE__, "GPU mesh upload submission could not enter residency.");
			}
			request.bUploadSubmitted = true;
		}
	}
}

void GpuMeshCache::PollResidency() noexcept
{
	ConsumeCompletedPreparations();
	m_residency.Poll(m_submissions);
	ActivateResidentMeshes();
	RemoveTerminalRequests();
}

void GpuMeshCache::RetainOnly(std::span<const GpuMeshHandle> handles) noexcept
{
	std::vector<std::uint64_t> wantedHandles;
	wantedHandles.reserve(handles.size());
	for (GpuMeshHandle handle : handles)
	{
		if (handle)
		{
			wantedHandles.push_back(handle.Value);
		}
	}

	std::sort(wantedHandles.begin(), wantedHandles.end());
	wantedHandles.erase(std::unique(wantedHandles.begin(), wantedHandles.end()), wantedHandles.end());

	const auto isWanted = [&wantedHandles](GpuMeshHandle handle) noexcept
	{
		return std::binary_search(wantedHandles.begin(), wantedHandles.end(), handle.Value);
	};

	for (auto& entry : m_requests)
	{
		MeshRequest& request = entry.second;
		request.bWanted = isWanted(request.Handle);
		if (!request.bWanted)
		{
			const bool bPreparing = m_residency.GetState(request.Generation) == AssetResidencyState::Decoding;
			if (bPreparing)
			{
				continue;
			}
			(void) m_residency.Cancel(request.Generation);
			if (request.Uploaded == nullptr)
			{
				request.Prepared.reset();
			}
		}
	}

	for (auto mesh = m_cache.begin(); mesh != m_cache.end();)
	{
		const GpuMeshHandle handle = mesh->second.Mesh->GetHandle();
		if (isWanted(handle))
		{
			++mesh;
			continue;
		}

		const auto sourceHandle = m_sourceHandles.find(mesh->second.Source);
		if (sourceHandle != m_sourceHandles.end() && sourceHandle->second == handle)
		{
			m_sourceHandles.erase(sourceHandle);
		}

		m_handles.erase(handle.Value);
		RetireActiveMesh(mesh->second);
		mesh = m_cache.erase(mesh);
	}
}
