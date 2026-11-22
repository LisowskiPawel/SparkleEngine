#include "PCH.h"
#include "Scene/GpuScene/GpuSceneSlotAllocator.h"

#include "RHI/Public/Commands/RhiCommandSubmissionService.h"
#include "Core/Public/Diagnostics/Logger.h"
#include "Core/Public/Diagnostics/Verify.h"

#include <algorithm>
#include <limits>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_gpuSceneSlotLogger, "Renderer.GpuScene");

GpuSceneSlotAllocator::GpuSceneSlotAllocator(RhiCommandSubmissionService* submissionService) noexcept :
    m_submissionService(submissionService)
{
}

std::uint32_t GpuSceneSlotAllocator::Allocate()
{
	DrainCompletedRetirements();
	if (m_availableSlots.empty())
	{
		if (m_nextSlot == (std::numeric_limits<std::uint32_t>::max)())
		{
			Diagnostics::Fatal(g_gpuSceneSlotLogger, __FILE__, __LINE__, "GPU-scene slot capacity exhausted.");
		}
		return m_nextSlot++;
	}

	const auto available = std::min_element(m_availableSlots.begin(), m_availableSlots.end());
	const std::uint32_t slot = *available;
	m_availableSlots.erase(available);
	return slot;
}

void GpuSceneSlotAllocator::Retire(std::uint32_t slot)
{
	RetiredSlot retired{.Slot = slot};
	if (m_submissionService != nullptr)
	{
		retired.LastUse = m_submissionService->CaptureLastSubmittedState();
	}

	if (IsComplete(retired))
	{
		m_availableSlots.push_back(slot);
		return;
	}
	m_retiredSlots.push_back(retired);
}

void GpuSceneSlotAllocator::DrainCompletedRetirements()
{
	for (std::size_t index = 0; index < m_retiredSlots.size();)
	{
		if (!IsComplete(m_retiredSlots[index]))
		{
			++index;
			continue;
		}
		m_availableSlots.push_back(m_retiredSlots[index].Slot);
		m_retiredSlots.erase(m_retiredSlots.begin() + index);
	}
}

bool GpuSceneSlotAllocator::IsComplete(const RetiredSlot& retired) const noexcept
{
	return m_submissionService == nullptr || m_submissionService->IsSubmissionStateComplete(retired.LastUse);
}
