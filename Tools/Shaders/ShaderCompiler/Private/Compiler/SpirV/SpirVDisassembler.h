#pragma once

#include <cstdint>
#include <span>
#include <string>

std::string DisassembleSpirV(std::span<const std::uint8_t> bytecode);
