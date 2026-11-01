#pragma once

#include "Core/Public/Console/ConsoleCommandRegistry.h"

#include <stop_token>

namespace SparkleLauncher
{
	ConsoleCommandResult RecompileEditorShaders(bool generateSymbols, std::stop_token cancellation);
}
