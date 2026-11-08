#pragma once

#include "Tasks/Public/TaskExecution.h"
#include "Textures/CookedTextureLoader.h"

#include <cstdint>
#include <filesystem>
#include <memory>

class TaskExecutor;
class TaskScope;

struct CookedTextureLoadPayload final
{
	CookedTextureFilePayload File;
	LoadedTextureData Texture;
};

TaskExecution LaunchCookedTextureLoad(
    TaskExecutor& taskExecutor,
    TaskScope& taskScope,
    const std::filesystem::path& path,
    const std::shared_ptr<CookedTextureLoadPayload>& payload,
    std::uint64_t maximumDecodedBytes);
