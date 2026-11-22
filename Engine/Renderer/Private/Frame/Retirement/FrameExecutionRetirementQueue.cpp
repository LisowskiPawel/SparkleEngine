#include "PCH.h"

#include "Frame/Retirement/FrameExecutionRetirementQueue.h"

#include "Frame/RenderFrame.h"
#include "FrameGraph/FrameGraph.h"
#include "RHI/Public/Device/RenderDeviceServices.h"

#include <algorithm>

FrameExecutionRetirementQueue::FrameExecutionRetirementQueue() noexcept = default;

FrameExecutionRetirementQueue::~FrameExecutionRetirementQueue() noexcept = default;

void FrameExecutionRetirementQueue::Retire(const RenderDeviceServices& deviceServices, std::unique_ptr<FrameGraph> graph, std::vector<std::unique_ptr<RenderFrame>> renderFrames) noexcept
{
	if (graph == nullptr)
	{
		return;
	}

	m_retiredExecutions.push_back(RetiredFrameExecution{.LastUse = deviceServices.CaptureLastSubmittedState(), .Graph = std::move(graph), .RenderFrames = std::move(renderFrames)});
}

void FrameExecutionRetirementQueue::Poll(const RenderDeviceServices& deviceServices) noexcept
{
	m_retiredExecutions.erase(
	    std::remove_if(
	        m_retiredExecutions.begin(),
	        m_retiredExecutions.end(),
	        [&deviceServices](const RetiredFrameExecution& execution) noexcept { return deviceServices.IsSubmissionStateComplete(execution.LastUse); }),
	    m_retiredExecutions.end());
}
