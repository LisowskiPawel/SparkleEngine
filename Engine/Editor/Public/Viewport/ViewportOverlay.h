#pragma once

#include "EditorAPI.h"

class EditorIconService;

// Optional controls drawn over the viewport surface at its upper right. UI installation transfers
// ownership; measurement, drawing, replacement and destruction run on the Editor
// thread with live ImGui. Destruction precedes backend/context teardown.
class SPARKLE_EDITOR_API ViewportOverlay
{
public:
	virtual ~ViewportOverlay() noexcept;

	ViewportOverlay(const ViewportOverlay&) = delete;
	ViewportOverlay& operator=(const ViewportOverlay&) = delete;
	ViewportOverlay(ViewportOverlay&&) = delete;
	ViewportOverlay& operator=(ViewportOverlay&&) = delete;

	virtual float MeasureWidth() const noexcept = 0;
	virtual void Draw(EditorIconService& icons, bool bDisableInteraction) noexcept = 0;

protected:
	ViewportOverlay() noexcept;
};
