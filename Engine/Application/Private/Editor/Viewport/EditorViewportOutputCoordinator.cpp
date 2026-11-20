#include "PCH.h"
#include "Renderer/Public/Viewport/ViewportPresentationSnapshot.h"

#include "Editor/Viewport/EditorViewportOutputCoordinator.h"

#include "Editor/ReferencePathTracer/ReferencePathTracerArtifactActions.h"
#include "Editor/Public/UI.h"
#include "Renderer.h"

#include <utility>

EditorViewportOutputCoordinator::EditorViewportOutputCoordinator(TaskExecutor& executor, TaskScope& scope) noexcept :
    m_presentationCapture(executor, scope),
    m_referenceArtifacts(executor, scope)
{
}

void EditorViewportOutputCoordinator::Update(UI& ui, Renderer& renderer)
{
	if (auto status = m_presentationCapture.Update(renderer))
	{
		ui.AppendConsoleOutput(std::move(*status), true);
	}
	const auto presentation = renderer.GetViewportPresentation();
	m_referenceArtifacts.Update(renderer, presentation.Products);
	ReportReferencePathTracerArtifactResult(ui, m_referenceArtifacts.TakeResult());
}

void EditorViewportOutputCoordinator::HandleAction(UI& ui, Renderer& renderer, std::uint64_t frameId)
{
	const ViewportOutputAction action = ui.ConsumeViewportOutputAction();
	if (action == ViewportOutputAction::CapturePresentation)
	{
		if (auto status = m_presentationCapture.Request(renderer, frameId))
		{
			ui.AppendConsoleOutput(std::move(*status), true);
		}
		return;
	}

	const auto presentation = renderer.GetViewportPresentation();
	ApplyReferencePathTracerArtifactAction(action, m_referenceArtifacts, renderer, presentation.Products);
	ReportReferencePathTracerArtifactResult(ui, m_referenceArtifacts.TakeResult());
}
