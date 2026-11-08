#pragma once

#include <cstdint>
#include <optional>
#include "Editor/ReferencePathTracer/ReferencePathTracerArtifact.h"

class ReferencePathTracerArtifactCoordinator;
class Renderer;
class UI;
struct ViewportRenderProducts;
enum class ViewportOutputAction : std::uint8_t;

void ApplyReferencePathTracerArtifactAction(ViewportOutputAction action, ReferencePathTracerArtifactCoordinator& artifacts, Renderer& renderer, const ViewportRenderProducts& products);
void ReportReferencePathTracerArtifactResult(UI& ui, std::optional<ReferencePathTracerArtifactWriteResult> result);
