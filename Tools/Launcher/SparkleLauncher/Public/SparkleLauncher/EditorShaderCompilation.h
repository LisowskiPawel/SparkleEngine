#pragma once

#include "Core/Public/Console/ConsoleOutput.h"

#include <stop_token>

namespace SparkleLauncher
{
	ConsoleCommandResult RecompileEditorShaders(bool generateSymbols, std::stop_token cancellation);
}
