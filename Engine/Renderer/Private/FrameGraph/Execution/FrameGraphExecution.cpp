#include "PCH.h"
#include "FrameGraph/FrameGraph.h"

#include "RHI/Public/Commands/RenderCommandList.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"
#include "FrameGraph/Diagnostics/FrameGraphExecutionDiagnostics.h"
#include "FrameGraph/Execution/FrameGraphSubmissionExecutor.h"
#include "RHI/Public/Commands/RhiCommandSubmissionService.h"
#include <algorithm>

void FrameGraph::Execute(const FrameGraphPlan& plan, RhiCommandSubmissionService& submissionService, enkiTaskScheduler* frameScheduler) const
{
	EnsureTransientResourcesMaterialized(plan);
	submissionService.PrepareCommandRecording();

	std::fill(m_submissionBatchTokens.begin(), m_submissionBatchTokens.end(), RhiSubmissionToken{});

	RenderCommandList& initialGraphicsCommandList = submissionService.GetCurrentGraphicsCommandList();
	RecordFrameBeginBarriers(plan, initialGraphicsCommandList);

	FrameGraphSubmissionExecutor submissionExecutor(*this, plan, submissionService, frameScheduler, m_submissionBatchTokens);
	RenderCommandList& finalGraphicsCommandList = submissionExecutor.Execute(initialGraphicsCommandList);
	RecordFrameEndBarriers(plan, finalGraphicsCommandList);

	CommitTextureHistories();
}

void FrameGraph::RecordFrameBeginBarriers(const FrameGraphPlan& plan, RenderCommandList& commandList) const
{
	FrameGraphExecutionDiagnostics diagnostics(m_renderHardwareInterface.GetDiagnostics(), commandList);
	if (!plan.initialTransientAliasingBarriers.empty())
	{
		diagnostics.InsertFrameBeginAliasingBarrierMarker();
	}

	EmitTransientAliasingBarriers(commandList, "FrameBegin", plan.initialTransientAliasingBarriers);
	EmitCompiledBarriers(commandList, "FrameBegin", plan.initialBarriers);
}

void FrameGraph::RecordFrameEndBarriers(const FrameGraphPlan& plan, RenderCommandList& commandList) const
{
	FrameGraphExecutionDiagnostics diagnostics(m_renderHardwareInterface.GetDiagnostics(), commandList);
	if (!plan.finalBarriers.empty())
	{
		diagnostics.InsertFrameEndResourceBarrierMarker();
	}

	EmitCompiledBarriers(commandList, "FrameEnd", plan.finalBarriers);
}
