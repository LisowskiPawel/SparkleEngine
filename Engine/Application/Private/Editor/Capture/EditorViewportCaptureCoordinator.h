#pragma once

#include "Editor/Capture/ViewportCaptureSlot.h"
#include "Editor/Operations/EditorOperationSlot.h"
#include "Core/Public/Console/ConsoleOutput.h"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>

class TaskExecutor;
class TaskScope;
class Renderer;

class EditorViewportCaptureCoordinator final
{
public:
	EditorViewportCaptureCoordinator(TaskExecutor& executor, TaskScope& scope) noexcept;

	std::optional<ConsoleOutputRecord> Request(Renderer& renderer, std::uint64_t frameId);
	std::optional<ConsoleOutputRecord> Update(Renderer& renderer);

private:
	std::filesystem::path BuildOutputPath(std::uint64_t frameId) const;

	EditorOperationSlot<ViewportCaptureResult> m_writeOperation;
	ViewportCaptureSlot m_capture;
	std::filesystem::path m_outputPath;
};
