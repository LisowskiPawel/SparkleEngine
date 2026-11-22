#pragma once

#include "Editor/Capture/ViewportCaptureSlot.h"
#include <memory>
#include <stop_token>
#include "Core/Public/Console/ConsoleOutput.h"

#include <cstdint>
#include <filesystem>
#include <optional>

struct enkiTaskScheduler;
class Renderer;

class EditorViewportCaptureCoordinator final
{
public:
	EditorViewportCaptureCoordinator(enkiTaskScheduler* io, std::stop_token stop) noexcept;
	~EditorViewportCaptureCoordinator();
	void Shutdown() noexcept;

	std::optional<ConsoleOutputRecord> Request(Renderer& renderer, std::uint64_t frameId);
	std::optional<ConsoleOutputRecord> Update(Renderer& renderer);

private:
	std::filesystem::path BuildOutputPath(std::uint64_t frameId) const;

	struct Write;
	static void WriteCapture(void* argument) noexcept;
	enkiTaskScheduler* m_io;
	std::stop_token m_stop;
	std::unique_ptr<Write> m_write;
	ViewportCaptureSlot m_capture;
	std::filesystem::path m_outputPath;
};
