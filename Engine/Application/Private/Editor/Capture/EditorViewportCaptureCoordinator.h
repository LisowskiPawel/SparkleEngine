#pragma once

#include "Editor/Capture/ViewportCaptureSlot.h"
#include "EditorOperations/EditorOperationSlot.h"
#include "Core/Public/Console/ConsoleOutput.h"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>

class EditorOperationRuntime;
class Renderer;

class EditorViewportCaptureCoordinator final
{
public:
	explicit EditorViewportCaptureCoordinator(EditorOperationRuntime& operations) noexcept;

	std::optional<ConsoleOutputRecord> Request(Renderer& renderer, std::uint64_t frameId);
	std::optional<ConsoleOutputRecord> Update(Renderer& renderer);

private:
	std::filesystem::path BuildOutputPath(std::uint64_t frameId) const;

	EditorOperationSlot<ViewportCaptureResult> m_writeOperation;
	ViewportCaptureSlot m_capture;
	std::filesystem::path m_outputPath;
};
