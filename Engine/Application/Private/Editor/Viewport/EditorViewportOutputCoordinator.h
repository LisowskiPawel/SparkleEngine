#pragma once

#include "Editor/Capture/EditorViewportCaptureCoordinator.h"
#include "Editor/ReferencePathTracer/ReferencePathTracerArtifactCoordinator.h"

#include <cstdint>

class TaskExecutor;
class TaskScope;
class Renderer;
class UI;

class EditorViewportOutputCoordinator final
{
public:
	EditorViewportOutputCoordinator(TaskExecutor& executor, TaskScope& scope) noexcept;

	EditorViewportOutputCoordinator(const EditorViewportOutputCoordinator&) = delete;
	EditorViewportOutputCoordinator& operator=(const EditorViewportOutputCoordinator&) = delete;

	void Update(UI& ui, Renderer& renderer);
	void HandleAction(UI& ui, Renderer& renderer, std::uint64_t frameId);

private:
	EditorViewportCaptureCoordinator m_presentationCapture;
	ReferencePathTracerArtifactCoordinator m_referenceArtifacts;
};
