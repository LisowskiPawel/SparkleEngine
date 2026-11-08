#pragma once

#include "Renderer/Public/Viewport/ViewportContracts.h"

#include <filesystem>
#include <stop_token>

namespace ViewportCaptureWriter
{
	ViewportCaptureResult Write(ViewportCaptureReadback readback, const std::filesystem::path& outputPath, std::stop_token cancellationToken);
}
