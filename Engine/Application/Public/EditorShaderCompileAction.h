#pragma once

#include "Core/Public/Console/ConsoleOutput.h"

#include <functional>
#include <stop_token>

// Tooling supplies this blocking operation. Application runs it on its existing
// bounded native I/O capacity; Engine does not resolve tools or build cook arguments.
using EditorShaderCompileAction = std::function<ConsoleCommandResult(bool bGenerateSymbols, std::stop_token cancellation)>;
