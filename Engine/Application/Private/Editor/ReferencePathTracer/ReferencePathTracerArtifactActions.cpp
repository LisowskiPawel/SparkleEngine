#include "PCH.h"

#include "Editor/ReferencePathTracer/ReferencePathTracerArtifactActions.h"

#include "Editor/ReferencePathTracer/ReferencePathTracerArtifactCoordinator.h"
#include "Editor/Public/Viewport/ViewportOutputAction.h"
#include "Editor/Public/UI.h"
#include "Core/Public/Strings/StringUtils.h"
#include <utility>

void ApplyReferencePathTracerArtifactAction(ViewportOutputAction action, ReferencePathTracerArtifactCoordinator& artifacts, Renderer& renderer, const ViewportRenderProducts& products)
{
	switch (action)
	{
		case ViewportOutputAction::SaveCurrentPrefix:
			artifacts.Request(ReferencePathTracerArtifactKind::PartialPrefix, renderer, products);
			break;
		case ViewportOutputAction::SaveComplete:
			artifacts.Request(ReferencePathTracerArtifactKind::Complete, renderer, products);
			break;
		case ViewportOutputAction::SaveCheckpoint:
			artifacts.Request(ReferencePathTracerArtifactKind::Checkpoint, renderer, products);
			break;
		case ViewportOutputAction::CapturePresentation:
		case ViewportOutputAction::None:
			break;
	}
}

void ReportReferencePathTracerArtifactResult(UI& ui, std::optional<ReferencePathTracerArtifactWriteResult> result)
{
	if (!result)
	{
		return;
	}
	const ConsoleCommandSeverity severity = result->bSucceeded ? ConsoleCommandSeverity::Info : ConsoleCommandSeverity::Error;

	std::string message;
	if (result->bSucceeded)
	{
		const std::string directory = Strings::ToNarrow(result->PublicationDirectory.wstring());
		message = "Reference artifact saved: " + directory;
	}
	else
	{
		message = "Reference artifact failed: " + std::move(result->ErrorMessage);
	}

	ui.AppendConsoleOutput({severity, std::move(message)}, true);
}
