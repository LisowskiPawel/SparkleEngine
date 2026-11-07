#pragma once

#include "Viewport/ViewportOutputAction.h"

struct ViewportRenderProgress;
struct ViewportRenderRequest;

void DrawReferencePathTracerOverlay(const ViewportRenderProgress& progress, ViewportRenderRequest& request, ViewportOutputAction& outputAction) noexcept;
