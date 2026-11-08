#pragma once

#include "Renderer/Public/Viewport/RenderViewMode.h"

#include <optional>

std::optional<RenderViewMode> DrawViewportViewModeMenu(RenderViewMode currentViewMode, bool bDisableInteraction, bool bCompact) noexcept;
