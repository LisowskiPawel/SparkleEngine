#pragma once

#include "Core/Public/Console/CVarControl.h"

class ConsoleCommandRegistry;

void RegisterConsoleBuiltinCommands(ConsoleCommandRegistry& commandRegistry, CVarControlExecutor executor);
