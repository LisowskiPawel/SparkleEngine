#pragma once

#include <filesystem>

bool IsShaderCookCancelled(const std::filesystem::path& signalPath);
void ThrowIfShaderCookCancelled(const std::filesystem::path& signalPath);
