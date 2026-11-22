#include "PCH.h"
#include "Meshes/GpuMeshCache.h"

#include "Core/Public/Diagnostics/Verify.h"
#include "Meshes/GpuMeshPreparation.h"
#include "RHI/Public/Commands/RhiCommandSubmissionService.h"
#include "Scene/Meshes/Mesh.h"
#include <TaskScheduler_c.h>
#include <new>

#include <algorithm>
#include <utility>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_gpuMeshCacheLogger, "Renderer.GpuMeshCache");

const GpuMesh* GpuMeshCache::Resolve(GpuMeshHandle handle) const noexcept
{
	if (!handle)
	{
		return nullptr;
	}

	const auto mesh = m_handles.find(handle.Value);
	return mesh != m_handles.end() ? mesh->second : nullptr;
}

bool GpuMeshCache::Contains(const Mesh& cpuMesh) const noexcept
{
	return m_sourceHandles.contains(&cpuMesh);
}

const GpuMesh* GpuMeshCache::Find(const Mesh& cpuMesh) const noexcept
{
	const auto source = m_sourceHandles.find(&cpuMesh);
	return source != m_sourceHandles.end() ? Resolve(source->second) : nullptr;
}

GpuMeshHandle GpuMeshCache::AllocateHandle()
{
	const GpuMeshHandle handle{m_nextGpuMeshHandle++};
	if (m_nextGpuMeshHandle == 0u)
	{
		Diagnostics::Fatal(g_gpuMeshCacheLogger, __FILE__, __LINE__, "GPU mesh handle space is exhausted.");
	}

	return handle;
}

GpuMeshCache::MeshRequest* GpuMeshCache::FindRequest(const CacheKey& key) noexcept
{
	const auto request = m_requests.find(key);
	return request != m_requests.end() ? &request->second : nullptr;
}

void GpuMeshCache::LaunchPendingPreparations()
{
	const std::size_t activePreparationCount = static_cast<std::size_t>(std::count_if(
	    m_requests.begin(),
	    m_requests.end(),
	    [this](const auto& entry) noexcept
	    {
		    const MeshRequest& request = entry.second;
		    return m_residency.GetState(request.Generation) == AssetResidencyState::Decoding;
	    }));

	std::size_t availableSlots = activePreparationCount < kMaximumConcurrentPreparations ? kMaximumConcurrentPreparations - activePreparationCount : 0;
	for (auto& entry : m_requests)
	{
		MeshRequest& request = entry.second;
		if (availableSlots == 0)
		{
			return;
		}
		const bool bPending = m_residency.GetState(request.Generation) == AssetResidencyState::Reading && request.bWanted;
		if (!bPending || request.Prepared == nullptr)
		{
			continue;
		}
		const std::uint64_t decodedBytes = CalculateGpuMeshDecodedByteSize(request.Source);
		if (decodedBytes > m_residency.GetBudget().MaximumDecodedBytes)
		{
			Diagnostics::Fatal(g_gpuMeshCacheLogger, __FILE__, __LINE__, "GPU mesh preparation exceeds the decoded-byte budget.");
		}
		if (!m_residency.BeginDecoding(request.Generation, decodedBytes))
		{
			continue;
		}

		LaunchPreparation(request);
		--availableSlots;
	}
}

void GpuMeshCache::PrepareMesh(void* argument) noexcept
{
	auto& request = *static_cast<MeshRequest*>(argument);
	if (request.Stop.stop_requested())
	{
		return;
	}
	try
	{
		*request.Prepared = PrepareGpuMesh(request.Source);
	}
	catch (...)
	{
		request.Failure = std::current_exception();
	}
}

void GpuMeshCache::LaunchPreparation(MeshRequest& request)
{
	if (m_cpu)
	{
		const auto config = enkiGetTaskSchedulerConfig(m_cpu);
		const auto worker = config.numExternalTaskThreads + 1 + static_cast<std::uint32_t>(request.Handle.Value % config.numTaskThreadsToCreate);
		request.Preparation = enkiCreatePinnedTask(m_cpu, PrepareMesh, worker);
		if (!request.Preparation)
		{
			(void) m_residency.Cancel(request.Generation);
			throw std::bad_alloc();
		}
		enkiSetArgsPinnedTask(request.Preparation, &request);
		enkiAddPinnedTask(m_cpu, request.Preparation);
	}
	else
	{
		PrepareMesh(&request);
	}
}

void GpuMeshCache::ConsumeCompletedPreparations() noexcept
{
	for (auto& entry : m_requests)
	{
		MeshRequest& request = entry.second;
		if (m_residency.GetState(request.Generation) != AssetResidencyState::Decoding)
		{
			continue;
		}
		if (request.Preparation && !enkiIsPinnedTaskComplete(m_cpu, request.Preparation))
		{
			continue;
		}
		if (request.Preparation)
		{
			enkiDeletePinnedTask(m_cpu, request.Preparation);
			request.Preparation = nullptr;
		}

		if (!request.bWanted)
		{
			(void) m_residency.Cancel(request.Generation);
			request.Prepared.reset();
			continue;
		}

		if (request.Failure)
		{
			try
			{
				std::rethrow_exception(request.Failure);
			}
			catch (const std::exception& error)
			{
				Diagnostics::Fatal(g_gpuMeshCacheLogger, __FILE__, __LINE__, error.what());
			}
			catch (...)
			{
				Diagnostics::Fatal(g_gpuMeshCacheLogger, __FILE__, __LINE__, "GPU mesh preparation failed.");
			}
		}

		if (request.Prepared == nullptr)
		{
			Diagnostics::Fatal(g_gpuMeshCacheLogger, __FILE__, __LINE__, "GPU mesh preparation produced no payload.");
		}

		const std::uint64_t decodedBytes = request.Prepared->GetDecodedByteSize();
		if (!m_residency.PublishReadyForUpload(request.Generation, decodedBytes, request.Prepared->GetResidentByteSize()))
		{
			Diagnostics::Fatal(g_gpuMeshCacheLogger, __FILE__, __LINE__, "GPU mesh generation exceeded residency capacity.");
		}
	}
}

void GpuMeshCache::ActivateResidentMeshes() noexcept
{
	for (auto& entry : m_requests)
	{
		MeshRequest& request = entry.second;
		if (request.Uploaded == nullptr)
		{
			continue;
		}

		const AssetResidencyState state = m_residency.GetState(request.Generation);
		if (!request.bWanted && state == AssetResidencyState::Retired)
		{
			request.Uploaded.reset();
			continue;
		}

		if (!request.bWanted || state != AssetResidencyState::Resident)
		{
			continue;
		}

		const Mesh* const source = request.Source.GetResource().get();
		const auto [active, bInserted] = m_cache.emplace(request.Key, ActiveMesh{.Generation = request.Generation, .Source = source, .Mesh = std::move(request.Uploaded)});
		if (!bInserted)
		{
			Diagnostics::Fatal(g_gpuMeshCacheLogger, __FILE__, __LINE__, "GPU mesh generation was activated twice.");
		}

		m_handles[request.Handle.Value] = active->second.Mesh.get();
		m_sourceHandles[source] = request.Handle;
	}
}

void GpuMeshCache::RemoveTerminalRequests() noexcept
{
	for (auto request = m_requests.begin(); request != m_requests.end();)
	{
		const AssetResidencyState state = m_residency.GetState(request->second.Generation);

		const bool bTerminalSecond = request->second.Preparation == nullptr && request->second.Prepared == nullptr;
		const bool bUploadMissing = bTerminalSecond && (request->second.Uploaded == nullptr);
		const bool bTerminalState = state == AssetResidencyState::Resident || state == AssetResidencyState::Retired;
		const bool bTerminal = bUploadMissing && bTerminalState;

		if (bTerminal)
		{
			request = m_requests.erase(request);
		}
		else
		{
			++request;
		}
	}
}

void GpuMeshCache::RetireActiveMesh(ActiveMesh& mesh) noexcept
{
	if (!m_residency.BeginEviction(mesh.Generation, m_submissions.CaptureLastSubmittedState()))
	{
		Diagnostics::Fatal(g_gpuMeshCacheLogger, __FILE__, __LINE__, "Resident GPU mesh could not enter eviction.");
	}
	mesh.Mesh.reset();
}
