#pragma once

#include <filesystem>
#include <vector>

struct ShaderDiagnosticsSnapshot;

std::vector<std::filesystem::path> FindShaderExportDirectories(const ShaderDiagnosticsSnapshot& diagnostics);
