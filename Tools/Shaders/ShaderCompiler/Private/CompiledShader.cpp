#include "PCH.h"

#include "CompiledShader.h"

#include <utility>

CompiledShader::CompiledShader(std::vector<std::uint8_t>&& bytecode) :
    m_bytecode(std::move(bytecode))
{
}
