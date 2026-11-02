#pragma once

#include "Core/Public/Console/ConsoleOutput.h"

#include <functional>
#include <stop_token>

// Tooling supplies this blocking operation. Application runs it on its existing
// editor operation scope; Engine does not resolve tools or build cook arguments.
using EditorShaderCompileAction = std::function<ConsoleCommandResult(bool generateSymbols, std::stop_token cancellation)>;
