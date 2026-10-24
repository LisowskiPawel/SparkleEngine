#pragma once

#include "Editor/Capture/ViewportCaptureSlot.h"
#include "EditorOperations/EditorOperationSlot.h"

#include <cstdint>
#include <filesystem>
#include <string>

class EditorOperationRuntime;
class EditorConsoleSystem;
class Renderer;

class EditorViewportCaptureCoordinator final
{
public:
	explicit EditorViewportCaptureCoordinator(EditorOperationRuntime& operations) noexcept;

	void Request(EditorConsoleSystem* console, Renderer& renderer, std::uint64_t frameId);
	void Update(EditorConsoleSystem* console, Renderer& renderer);

private:
	std::filesystem::path BuildOutputPath(std::uint64_t frameId) const;
	static void PublishStatus(EditorConsoleSystem* console, std::string status, bool failed);

	EditorOperationSlot<ViewportCaptureResult> m_writeOperation;
	ViewportCaptureSlot m_capture;
	std::filesystem::path m_outputPath;
};
