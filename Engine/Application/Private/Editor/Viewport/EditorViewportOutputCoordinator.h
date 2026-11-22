#pragma once

#include "Editor/Capture/EditorViewportCaptureCoordinator.h"
#include "Editor/ReferencePathTracer/ReferencePathTracerArtifactCoordinator.h"

#include <cstdint>

struct enkiTaskScheduler;
class Renderer;
class UI;

class EditorViewportOutputCoordinator final
{
public:
	EditorViewportOutputCoordinator(enkiTaskScheduler* io, std::stop_token stop) noexcept;

	EditorViewportOutputCoordinator(const EditorViewportOutputCoordinator&) = delete;
	EditorViewportOutputCoordinator& operator=(const EditorViewportOutputCoordinator&) = delete;

	void Shutdown() noexcept;
	void Update(UI& ui, Renderer& renderer);
	void HandleAction(UI& ui, Renderer& renderer, std::uint64_t frameId);

private:
	EditorViewportCaptureCoordinator m_presentationCapture;
	ReferencePathTracerArtifactCoordinator m_referenceArtifacts;
};
