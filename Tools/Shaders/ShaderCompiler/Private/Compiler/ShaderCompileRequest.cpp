#include "PCH.h"

#include "Compiler/ShaderCompileRequest.h"

ShaderCompileRequest::ShaderCompileRequest(const ShaderSourceMountTable& sourceMounts) noexcept :
    SourceMounts(sourceMounts)
{
}
