#include "../../PCH.h"
#include "Frame/FramePipeline.h"

#include "FrameGraph/FrameGraph.h"
#include "RHI/Public/Device/RenderDeviceServices.h"

void FramePipeline::ExecuteFrame()
{
	if (!m_bFrameGraphExecutable)
	{
		return;
	}
	m_frameGraph->Setup();
	const FrameGraphPlan& plan = m_frameGraph->Compile();
	m_frameGraph->PreparePasses();
	m_frameGraph->Execute(plan, m_deviceServices, m_taskExecutor);
}
