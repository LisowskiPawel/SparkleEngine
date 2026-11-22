#pragma once

#include <cstdint>
#include <span>
#include <string>

struct IDxcUtils;

std::string ReadDxilSymbolName(IDxcUtils& utils, std::span<const std::uint8_t> code, std::span<const std::uint8_t> symbols);
std::string DisassembleDxil(std::span<const std::uint8_t> code);
