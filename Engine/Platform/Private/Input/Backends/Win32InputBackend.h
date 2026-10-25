#pragma once

#include <cstdint>

struct InputBackendResult;

InputBackendResult ProcessWin32InputMessage(std::uint32_t message, std::uintptr_t parameter1, std::intptr_t parameter2);
