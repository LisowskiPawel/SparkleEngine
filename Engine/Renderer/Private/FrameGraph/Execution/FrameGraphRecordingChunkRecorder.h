#pragma once

#include "FrameGraph/Compiler/FrameGraphPlan.h"

class FrameGraph;
class FrameGraphExecutionDiagnostics;
class RenderCommandContext;
class RenderCommandList;
class ScopedGpuScope;

struct RecordingChunk;

class FrameGraphRecordingChunkRecorder final
{
public:
	FrameGraphRecordingChunkRecorder(const FrameGraph& frameGraph, const FrameGraphPlan& plan) noexcept;

	void Record(const RecordingChunk& chunk, RenderCommandList& commandList) const;

private:
	ScopedGpuScope BeginChunkScope(const RecordingChunk& chunk, RenderCommandList& commands) const;
	void RecordGroup(const RecordingGroup& group, RenderCommandList& commandList, RenderCommandContext& commands, FrameGraphExecutionDiagnostics& graphDiagnostics) const;
	void RecordPass(FrameGraphPassIndex passIndex, RenderCommandList& commandList, RenderCommandContext& commands, FrameGraphExecutionDiagnostics& graphDiagnostics) const;
	void TrackPassResources(const FrameGraphPassNode& pass, RenderCommandList& commandList) const;

	const FrameGraph& m_frameGraph;
	const FrameGraphPlan& m_plan;
};
