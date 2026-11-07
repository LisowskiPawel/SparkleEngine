#pragma once

class EditorViewportSession;
struct EngineRenderingSettingsState;

namespace ViewportCameraProperties
{
	void OpenPopup() noexcept;
	void BuildPopup(EditorViewportSession& viewportSession, const EngineRenderingSettingsState& renderingDefaults, bool disableInteraction) noexcept;
}
