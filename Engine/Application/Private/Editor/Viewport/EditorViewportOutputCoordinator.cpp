#include "PCH.h"

#include "Editor/Viewport/EditorViewportOutputCoordinator.h"

#include "Editor/Capture/EditorViewportCaptureCoordinator.h"
#include "Editor/ReferencePathTracer/ReferencePathTracerArtifactActions.h"
#include "Editor/ReferencePathTracer/ReferencePathTracerArtifactCoordinator.h"
#include "Editor/Public/UI.h"
#include "Renderer.h"

#include <utility>

struct EditorViewportOutputCoordinator::State final
{
	explicit State(EditorOperationRuntime& operations) noexcept :
	    PresentationCapture(operations),
	    ReferenceArtifacts(operations)
	{
	}

	EditorViewportCaptureCoordinator PresentationCapture;
	ReferencePathTracerArtifactCoordinator ReferenceArtifacts;
};

EditorViewportOutputCoordinator::EditorViewportOutputCoordinator(EditorOperationRuntime& operations) noexcept :
    m_state(std::make_unique<State>(operations))
{
}

EditorViewportOutputCoordinator::~EditorViewportOutputCoordinator() noexcept = default;

void EditorViewportOutputCoordinator::Update(UI& ui, Renderer& renderer)
{
	if (auto status = m_state->PresentationCapture.Update(renderer))
	{
		ui.AppendConsoleOutput(std::move(*status), true);
	}
	m_state->ReferenceArtifacts.Update(renderer, renderer.GetViewportPresentation().Products);
}

void EditorViewportOutputCoordinator::HandleAction(UI& ui, Renderer& renderer, std::uint64_t frameId)
{
	const ViewportOutputAction action = ui.ConsumeViewportOutputAction();
	if (action == ViewportOutputAction::CapturePresentation)
	{
		if (auto status = m_state->PresentationCapture.Request(renderer, frameId))
		{
			ui.AppendConsoleOutput(std::move(*status), true);
		}
		return;
	}
	ApplyReferencePathTracerArtifactAction(action, m_state->ReferenceArtifacts, renderer, renderer.GetViewportPresentation().Products);
}
