#pragma once

#include <string>
#include <vector>

struct ShaderCompileRequest;

// Caller-owned wide strings must outlive the borrowed argument pointers.
void BuildDxcCompileArguments(
    const ShaderCompileRequest& request,
    const std::wstring& wSourcePath,
    const std::wstring& wEntryPoint,
    const std::wstring& wTargetProfile,
    std::vector<std::wstring>& wDefines,
    std::vector<const wchar_t*>& outArgs);
