#pragma once

#include "Core/Public/CoreAPI.h"
#include "Core/Public/Console/CVarControl.h"

class ConsoleCommandRegistry;

SPARKLE_CORE_API void RegisterConsoleBuiltinCommands(ConsoleCommandRegistry& commandRegistry, CVarControlExecutor executor);
